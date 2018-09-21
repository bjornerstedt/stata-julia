#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"


int variables(jl_value_t *stata, jl_value_t *stata_data, int update) {
	if (!update) {
		create_selection(stata);
	}
	int i = 1;
	// Loop through all variables
	char* name = getNameFromList(stata_data, "variables", 0, i);
    while( strlen(name) ) {
		if (update) {
			// Check whether variable should be updated
			jl_array_t *x = call_julia("StataJulia", "isSetVar", stata_data, jl_cstr_to_string(name), NULL );
			if( jl_exception_occurred() || x == NULL ) {
				char buf[80];
				snprintf(buf, 80, "ERROR: %s\n", jl_typeof_str(jl_exception_occurred()));
				SF_display(buf);
				return 4323;
			}
			// Return if it should not be updated
			if(jl_unbox_int32(x)) {
				int ret = set_variable(stata, name, i );
				if(ret) {
					SF_error("Could not set Stata var\n");
					return 2347;
				}
			}
		} else {
			int ret = get_variable(stata, name, i );
			if( ret) {
				char buf[80];
				snprintf(buf, 80, "Could not get Stata var: %s, %d\n", name, i);
				SF_error(buf);
				return ret;
			}
		}
		name = getNameFromList(stata_data, "variables", 0, ++i);
    }
    return 0;
}

int matrices(jl_value_t *stata, jl_value_t *stata_data, int update) {
	int i = 1;
	int rv;
	char* name = getNameFromList(stata_data, "matrices", update, i++);
    while( strlen(name) ) {
		if((rv = matrix(stata, name, update)) )
			return rv;
		name = getNameFromList(stata_data, "matrices", update, i++);
    }
    return 0;
}

int macros(jl_value_t *stata, jl_value_t *stata_data, int update) {
	char *ret = NULL;
	char str[200];
	char buf[200];
	// Variable not required to be defined
	int rc = 0;
	char command[80];
	int i = 1;
	char* name = getNameFromList(stata_data, "macros", update, i++);
    while( strlen(name) ) {
		if (update) {
			char *content;
			jl_value_t* x = call_julia("StataJulia", "getMacro", stata, jl_cstr_to_string(name), NULL );
			if ( x == NULL  || jl_exception_occurred()) {
				snprintf(buf, 80, "Getting macro from Julia failed: %s\n", name);
				SF_error(buf);
				return 3296;
			}
			content = jl_string_ptr(x);
			if ((rc = SF_macro_save(name, (char*)content)) ){
				snprintf(buf, 80, "Saving macro to Stata failed: %s\n", name);
				SF_error(buf);
				return rc;
			}
		} else {
			char content[80];
			if ((rc = SF_macro_use(name, (char *)content, 80) ) ) {
				snprintf(buf, 80, "Getting macro from Stata failed: %s\n", name);
				SF_error(buf);
				return rc;
			}
			call_julia("StataJulia", "addMacro", stata, jl_cstr_to_string(name), jl_cstr_to_string(content) );
			if (jl_exception_occurred()) {
				snprintf(buf, 80, "Setting macro in Julia failed: %s\n", name);
				SF_error(buf);
				return 3296;
			}
		}
		name = getNameFromList(stata_data, "macros", update, i++);
    }
    return 0;
}

int scalars(jl_value_t *stata, jl_value_t *stata_data, int update) {
	char *ret = NULL;
	char str[200];
	char buf[200];
	int rc = 0;
	ST_double d;
	// Variable not required to be defined
	int i = 1;
	char* name = getNameFromList(stata_data, "scalars", update, i++);
    while( strlen(name) ) {
		if (update) {
			jl_value_t *x;
			// TODO: only floats work
			x = call_julia("StataJulia", "getScalar", stata, jl_cstr_to_string(name), NULL );
			if( x == NULL  || jl_exception_occurred()) {
				snprintf(buf, 80, "Could not get scalar: %s\n", name);
				SF_error(buf);
				return 5216;
			}

			d = jl_unbox_float64(x);
			if((rc = SF_scal_save(name, d))) {
				snprintf(buf, 80, "Could not save scalar: %s", name);
				SF_error(buf);
				return rc;
			}
		} else {
			if((rc = SF_scal_use(name, &d))) return(rc) ;   /* read scalar */
			jl_value_t *x = jl_box_float64(d);
			if( call_julia("StataJulia", "addScalar", stata, jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) {
				snprintf(buf, 80, "addScalar error: Could not add scalar: %s\n", name);
				SF_error(buf);
				return 3216;
			}
		}
		name = getNameFromList(stata_data, "scalars", update, i++);
    }
    return 0;
}


// ---------------- Get individual values --------------------

// Get Stata global array by name
int matrix(jl_value_t *stata, char* name, int update) {
	char errbuf[80] ;
	ST_int i, j;
	ST_retcode rc ;
	ST_int rows, cols;
	rows = SF_row(name);
	cols = SF_col(name);

	if (rows == 0) {
	  	snprintf(errbuf, 80, "Could not get Stata matrix %s to write to\n", name) ;
	  	SF_display(errbuf);
		return 161;
	}

	if (update) {
		ST_double z;
		jl_value_t *x = call_julia("StataJulia", "getMatrix", stata, jl_cstr_to_string(name), NULL);
		if( x == NULL || jl_exception_occurred()) {
			snprintf(errbuf, 80, "Could not get array from Julia: %s\n", name);
			SF_error(errbuf);
			return 177;
		}
		// Get number of dimensions
		int ndims = jl_array_ndims(x);
		size_t rowsj = jl_array_dim(x, 0);
		size_t colsj = jl_array_dim(x, 1);
		if (rowsj != rows || colsj != cols) {
			snprintf(errbuf, 80, "ERROR: matrix dimensions for do not match for: %s", name);
			return 929;
		}

		// Get array pointer
	    double *xData = (double*)jl_array_data(x);
		for( i = 0; i < rows; i++) {
			for( j = 0; j < cols; j++) {
				// NOTE that Stata uses 1 based index!
				z = xData[j + cols * i];
				if((rc = SF_mat_store(name, i + 1, j + 1, z))) return(rc) ;
			}
		}
	} else {
		jl_array_t *x;
		if( (x = create_2D(rows, cols)) == NULL ) return 4343;
		// Get array pointer
	    double *xData = (double*)jl_array_data(x);
		ST_double z;
		for( i = 0; i < rows; i++) {
			for( j = 0; j < cols; j++) {
				// NOTE that Stata uses 1 based index!
					if(SF_mat_el(name, i + 1, j + 1, &z)) return(12312) ;
					xData[j + cols*i] = z;
			}
		}
		if( call_julia("StataJulia", "addMatrix", stata, jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) {
			SF_error("Could not add array to Julia\n");
			return 3214;
		}
	}
	return 0;
}

// //////////////////////////////////////////////////////////////////


int create_selection(jl_value_t *stata) {
	// Create variable touse
	ST_int i;
	ST_retcode rc ;
	jl_array_t *x;
	if( (x = create_2D(SF_in2(), 1)) == NULL ) return 6544;
	// TODO: Should it be a 1d array?
    double *xData = (double*)jl_array_data(x);
	for( i = 0; i < SF_in1() - 1; i++) {
		xData[i] = 0.0;
	}
	for( i = SF_in1() - 1; i < SF_in2(); i++) {
		xData[i] = (SF_ifobs(i + 1))?1.0:0.0;
	}

	if( call_julia("StataJulia", "addVariable", stata, jl_cstr_to_string("touse") , x ) == NULL ) {
		SF_error("Could not add Julia var touse\n");
		return 322;
	}
	return 0;
}


// Get Stata global array by name
int get_variable(jl_value_t *stata, char* name, int var_index) {
	ST_int i, j, rows;
	ST_retcode rc ;
	rows = SF_in2() ;

	if (rows == 0) {
		char errbuf[80] ;
	    snprintf(errbuf, 80, "Could not get Stata dataset column: %s\n", name) ;
	    SF_display(errbuf);
		return 654;
	}

	jl_array_t *x;
	if( (x = create_2D(rows, 1)) == NULL ) {
		SF_error("Could not allocate memory\n");
		return 755;
	}
	// TODO: Should it be a 1d array?
    double *xData = (double*)jl_array_data(x);
	ST_double z;
	for( i = 0; i < rows; i++) {
		// NOTE that Stata uses 1 based index and col row order!
		if(SF_vdata(var_index, i + 1, &z)) {
			SF_display("get_variable Index Error\n");
			return(876);
		}
		xData[i] = z; // Setting column vector
	}
	if( call_julia("StataJulia", "addVariable", stata, jl_cstr_to_string(name) , x ) == NULL ) {
		SF_error("Could not add Julia var\n");
		return 322;
	}
	return 0;
}

// Set either by name (matrices) or by index (variables)
int set_variable(jl_value_t *stata, char* name, int var_index) {
	char errbuf[80] ;
	ST_int i, j, rows;
	rows = SF_in2() ;
	ST_retcode rc ;
	ST_double z;

	jl_array_t *x = call_julia("StataJulia", "getVariable", stata, jl_cstr_to_string(name), NULL );
	if(jl_exception_occurred() || x == NULL ) {
		SF_error("Could not get Julia var\n");
		return 3241;
	}

	if (rows == 0) {
		snprintf(errbuf, 80, "Could not get Stata variable for setting: %s\n", name) ;
		SF_display(errbuf);
		return 99;
	}
	// Get number of dimensions
	int ndims = jl_array_ndims(x);
	size_t rowsj = jl_array_dim(x, 0);
	size_t colsj = jl_array_dim(x, 1);
	if (rowsj != rows || colsj != 1) {
		snprintf(errbuf, 80, "ERROR: variable dimensions for do not match for: %s", name);
		SF_display(errbuf);
		return 99;
	}
	// Get array pointer
	double *xData = (double*)jl_array_data(x);
	for( i = 0; i < rows; i++) {
		// NOTE that Stata uses 1 based index and col row order!
		z = xData[i];
		if((rc = SF_vstore(var_index, i + 1,  z))) return(rc) ;
	}
	return 0;
}
