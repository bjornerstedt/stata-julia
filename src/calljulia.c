#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

// Invoked by main funcname stata_call
int process(char *funcname)
{
		int rc = 0;
		if( (rc = get_macros()) )  return rc ;
		if( (rc = get_scalars()) )  return rc ;
		if( (rc = get_matrices()) )  return rc ;
		if( (rc = get_variables()) )  return rc ;
		// jexec("printToBuffer()");
		if (call_julia(NULL, funcname, NULL, NULL) == NULL){
			char buf[80] ;
			snprintf(buf, 80, "Could not run Julia function: %s\n", funcname);
			SF_error(buf);
		}
		if( (rc = set_matrices()) )  return rc ;
		if( (rc = set_macros()) )  return rc ;
		if( (rc = set_scalars()) )  return rc ;
		if( (rc = set_variables()) )  return rc ;
		rc = displayPrintBuffer();
		return rc;
}

int get_variables() {
	char* str = NULL;
	// Variable not required to be defined
	if( get_julia_string("stata_init[\"get_variables\"]", &str) )
		return 0;
	jl_value_t *x = NULL;
	int i = 1;
	char *name = " ";
	char command[80];
	snprintf(command, 80, "StataJulia.nameGetVar(%d)", i);
	get_julia_string(command, &name);
    while( strlen(name) ) {
		if( (x = (jl_value_t *)get_variable(name, i) ) == NULL) {
			SF_error("Could not get Stata var\n");
			return 234;
		}
		if( call_julia("StataJulia", "addVariable", jl_cstr_to_string(name) , x ) == NULL ) {
			SF_error("Could not add Julia var\n");
			return 322;
		}
		snprintf(command, 80, "StataJulia.nameGetVar(%d)", ++i);
		get_julia_string(command, &name);
    }
    return 0;
}

int set_variables() {
	char* str = NULL;
	int rc = 0;
	// Variable not required to be defined
	if( get_julia_string("stata_init[\"get_variables\"]", &str) )
		return 0;
	jl_value_t *x = NULL;
	int i = 1;
	char *name = " ";
	char command[80];
	snprintf(command, 80, "StataJulia.nameGetVar(%d)", i);
	if( (rc = get_julia_string(command, &name)) ) return rc;
    while( strlen(name) && i < 4) {
		// Check whether variable should be updated
		snprintf(command, 80, "StataJulia.isSetVar(\"%s\")", name);
		if( (x = jl_eval_string(command)) == NULL ) {
			SF_error("Should not happen!\n");
			return 10101;
		}
		// Update if it should
		if(jl_unbox_int32(x)) {
			snprintf(command, 80, "StataJulia.getVariable(\"%s\")", name);
			if( (x = jl_eval_string(command)) == NULL ) {
				SF_error("Could not get Julia var\n");
				return 321;
			}
			if(set_variable(name, i, (jl_array_t *)x)) {
				SF_error("Could not set Stata var\n");
				return 2347;
			}
		}
		// Get next variable
		snprintf(command, 80, "StataJulia.nameGetVar(%d)", ++i);
		if( (rc = get_julia_string(command, &name)) ) return rc;
    }
    return 0;
}

int get_matrices() {
	char *ret = NULL;
	char str[200];
	if( get_julia_string("stata_init[\"get_matrices\"]", &ret) )
		return 0; // Variable not required to be defined
	strcpy(str, ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		jl_array_t *x;
		if((x = get_matrix(name)) == NULL)
			return 1011;
		if( call_julia("StataJulia", "addMatrix", jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) {
			SF_error("Could not add array to Julia\n");
			return 3214;
		}
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int set_matrices() {
	char *ret = NULL;
	char str[200];
	char buf[200];
	if( get_julia_string("stata_init[\"set_matrices\"]", &ret) )
		return 0; // Variable not required to be defined
	strcpy(str, ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char command[80];
		snprintf(command, 80, "StataJulia.getMatrix(\"%s\")", name);
		jl_value_t *x = jl_eval_string(command);
	  	if (jl_exception_occurred()) {
			snprintf(buf, 80, "Could not get array from Julia: %s\n", name);
			SF_error(buf);
	  		return 177;
	  	}
		int rc = 0;
		if ((rc = set_matrix(name, (jl_array_t *)x) ) ){
			snprintf(buf, 80, "Setting Stata matrix failed: %s\n", name);
			SF_error(buf);
			return rc;
		}
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int get_macros() {
	char *ret = NULL;
	char str[200];
	char buf[200];
	// Variable not required to be defined
	if( get_julia_string("stata_init[\"get_macros\"]", &ret) )
		return 0;
	strcpy(str, ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char content[80];
		char command[80];
		int rc = 0;
		if ((rc = SF_macro_use(name, (char *)content, 80) ) ){
			snprintf(buf, 80, "Getting macro from Julia failed: %s\n", name);
			SF_error(buf);
			return rc;
		}
		snprintf(command, 80, "StataJulia.addMacro(\"%s\", \"%s\")", name, content);
		jl_eval_string(command);
		if (jl_exception_occurred()) {
			snprintf(buf, 80, "Setting macro in Julia failed: %s\n", name);
			SF_error(buf);
			return 3296;
		}
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int set_macros() {
	char *ret = NULL;
	char str[200];
	char buf[200];
	// Variable not required to be defined
	if( get_julia_string("stata_init[\"set_macros\"]", &ret) )
		return 0;
	strcpy(str, ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		const char* content;
		char command[80];
		snprintf(command, 80, "StataJulia.getMacro(\"%s\")", name);
		jl_value_t* ret = jl_eval_string(command);
		if (jl_exception_occurred()) {
			snprintf(buf, 80, "Getting macro from Julia failed: %s\n", name);
			SF_error(buf);
			return 3296;
		}
		content = jl_string_ptr(ret);
		int rc = 0;
		if ((rc = SF_macro_save(name, (char*)content)) ){
			snprintf(buf, 80, "Getting macro from Julia failed: %s\n", name);
			SF_error(buf);
			return rc;
		}
        name = strtok(NULL, " ");
    }
    return 0;
}

int get_scalars() {
	char *ret = NULL;
	char str[200];
	char buf[200];
	int rc = 0;
	ST_double d;
	// Variable not required to be defined
	if( get_julia_string("stata_init[\"get_scalars\"]", &ret) )
		return 0;
	strcpy(str, ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		if((rc = SF_scal_use(name, &d))) return(rc) ;   /* read scalar */
		jl_value_t *x = jl_box_float64(d);
		if( call_julia("StataJulia", "addScalar", jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) {
			snprintf(buf, 80, "Could not add scalar: %s\n", name);
			SF_error(buf);
			return 3216;
		}
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int set_scalars() {
	char *ret = NULL;
	char str[200];
	char buf[200];
	int rc = 0;
	ST_double d = 0;
	// Variable not required to be defined
	if( get_julia_string("stata_init[\"set_scalars\"]", &ret) )
		return 0;
	strcpy(str, ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		jl_value_t *x;
		// TODO: only floats work
		char command[80];
		snprintf(command, 80, "StataJulia.getScalar(\"%s\")", name);
		if( (x = jl_eval_string(command)) == NULL ) return 321;

		d = jl_unbox_float64(x);
		if((rc = SF_scal_save(name, d))) {
			snprintf(buf, 80, "Could not save scalar: %s", name);
			SF_error(buf);
			return rc;
		}
        name = strtok(NULL, " "); // Get next name
    }
    return 0;
}


// ---------------- Get individual values --------------------

// Get Stata global array by name
jl_array_t *get_variable(char* name, int var_index) {
	ST_int i, j, rows;
	ST_retcode rc ;
	rows = SF_in2() ;

	if (rows == 0) {
		char errbuf[80] ;
	    snprintf(errbuf, 80, "Could not get matrix: %s\n", name) ;
	    SF_display(errbuf);
		return NULL;
	}

	jl_array_t *x;
	if( (x = create_2D(rows, 1)) == NULL ) return NULL;
	// TODO: Should it be a 1d array?
    double *xData = (double*)jl_array_data(x);
	ST_double z;
	for( i = 0; i < rows; i++) {
		// NOTE that Stata uses 1 based index and col row order!
		if(SF_vdata(var_index, i + 1, &z)) {
			SF_display("Error");
			return(NULL);
		}
		xData[i] = z; // Setting column vector
	}
	return x;
}

// Get Stata global array by name
jl_array_t *get_matrix(char* name) {
	ST_int i, j;
	ST_retcode rc ;
	ST_int rows, cols;
	rows = SF_row(name);
	cols = SF_col(name);

	if (rows == 0) {
		char errbuf[80] ;
	  	snprintf(errbuf, 80, "Could not get matrix: %s\n", name) ;
	  	SF_display(errbuf);
		return NULL;
	}

	jl_array_t *x;
	if( (x = create_2D(rows, cols)) == NULL ) return NULL;

	// Get array pointer
    double *xData = (double*)jl_array_data(x);
	ST_double z;
	for( i = 0; i < rows; i++) {
		for( j = 0; j < cols; j++) {
			// NOTE that Stata uses 1 based index!
				if(SF_mat_el(name, i + 1, j + 1, &z)) return(NULL) ;
				xData[j + cols*i] = z;
		}
	}
	return x;
}

// Set either by name (matrices) or by index (variables)
int set_variable(char* name, int var_index, jl_array_t *x) {
	char errbuf[80] ;
	ST_int i, j, rows;
	rows = SF_in2() ;
	ST_retcode rc ;
	ST_double z;

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

// Set either by name (matrices) or by index (variables)
int set_matrix(char* name, jl_array_t *x) {
	char errbuf[80] ;
	ST_int rows, cols;
	rows = SF_row(name);
	cols = SF_col(name);

	ST_int i, j;
	ST_retcode rc ;
	ST_double z;

	if (rows == 0) {
	    snprintf(errbuf, 80, "Could not get matrix: %s\n", name) ;
	    SF_display(errbuf);
		return 989;
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
	return 0;
}
