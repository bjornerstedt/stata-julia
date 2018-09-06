#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

// Invoked by main funcname stata_call
int SJ_process(char *funcname, char *varlist)
{
		int rc = 0;
		SJ_get_macros();
		SJ_get_scalars();
		SJ_get_matrices();
		// SJ_get_dataset();
		SJ_get_variables();
		jexec("printToBuffer()");

		jl_function_t *func = jl_get_function(jl_current_module, funcname);
		if (jl_exception_occurred() || func == NULL) {
	        SF_error("function not found\n");
			return 1010;
		}
		jl_call0(func);
		SJ_set_matrices();
		SJ_set_macros();
		SJ_set_scalars();
		SJ_set_variables();
		displayPrintBuffer();
		return rc;
}

int SJ_get_variables() {
	char* str = NULL;
	// Variable not required to be defined
	if( get_julia_string("get_variables", &str) )
		return 0;
	jl_value_t *x = NULL;
	int i = 1;
	char *name = " ";
	char command[80];
	snprintf(command, 80, "nameGetVar(%d)", i);
	get_julia_string(command, &name);
    while( strlen(name) ) {
		if( (x = SJ_get_variable(name, i) ) == NULL) {
			SF_error("Could not get Stata var\n");
			return 234;
		}
		if( call_julia("addVariable", jl_cstr_to_string(name) , (jl_value_t *)x ) == NULL ) {
			SF_error("Could not add Julia var\n");
			return 322;
		}
		snprintf(command, 80, "nameGetVar(%d)", ++i);
		get_julia_string(command, &name);
    }
    return 0;
}

int SJ_set_variables() {
	char* str = NULL;
	// Variable not required to be defined
	if( get_julia_string("get_variables", &str) )
		return 0;
	jl_value_t *x = NULL;
	int i = 1;
	char *name = " ";
	char command[80];
	snprintf(command, 80, "nameGetVar(%d)", i);
	get_julia_string(command, &name);
    while( strlen(name) && i < 4) {
		// Check whether variable should be updated
		snprintf(command, 80, "isSetVar(\"%s\")", name);
		if( (x = jl_eval_string(command)) == NULL ) {
			SF_error("Should not happen!\n");
			return 10101;
		}
		// Update if it should 
		if(jl_unbox_int32(x)) {
			snprintf(command, 80, "getVariable(\"%s\")", name);
			if( (x = jl_eval_string(command)) == NULL ) {
				SF_error("Could not get Julia var\n");
				return 321;
			}
			if(SJ_set_variable(name, i, (jl_array_t *)x)) {
				SF_error("Could not set Stata var\n");
				return 234;
			}
		}
		// Get next variable
		snprintf(command, 80, "nameGetVar(%d)", ++i);
		get_julia_string(command, &name);
    }
    return 0;
}

int SJ_get_dataset() {
	ST_int cols = SF_nvars();
	if (cols == 0) {
		return 1;
	}
	ST_int rows = SF_in2();
	jl_array_t* x;
	if( (x = create_2D(rows, cols)) == NULL ) return 1293;
	double *xData = (double*)jl_array_data(x);
	ST_double z;
	ST_retcode rc ;
	for(ST_int i = 0; i < rows; i++) {
		for(ST_int j = 0; j < cols; j++) {
			// NOTE that Stata uses 1 based index!
			if((rc = SF_vdata( i + 1, j + 1, &z))) return(rc) ;
			xData[j + cols*i] = z;
		}
	}
	if( call_julia("addDataset", (jl_value_t *)x, NULL ) == NULL ) return 321;
    return 0;
}

int SJ_get_matrices() {
	char *str = NULL;
	// Variable not required to be defined
	if( get_julia_string("get_matrices", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		jl_array_t *x = SJ_get_matrix(name);
		jl_function_t *func = jl_get_function(jl_current_module, "addMatrix");
		if (jl_exception_occurred() || func == NULL) {
	        SF_error("function addMatrix not found\n");
			return 1010;
		}
	  	jl_value_t *jname = jl_cstr_to_string(name);
	    jl_call2(func, jname, (jl_value_t *)x);
		if (jl_exception_occurred())
	        SF_display("setting of matrix in Julia failed\n");
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_set_matrices() {
	char *str = NULL;

	// Variable not required to be defined
	if( get_julia_string("set_matrices", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char command[80];
		snprintf(command, 80, "getMatrix(\"%s\")", name);
		jl_value_t *x = jl_eval_string(command);
	  	if (jl_exception_occurred()) {
	  		SF_error("Could not get array from Julia\n");
	  		return 1;
	  	}
		SJ_set_matrix(name, (jl_array_t *)x);
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_get_macros() {
	char *str = NULL;
	// Variable not required to be defined
	if( get_julia_string("get_macros", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char content[80];
		char command[80];
		SF_macro_use(name, content, 80);
		snprintf(command, 80, "addMacro(\"%s\", \"%s\")", name, content);
		jl_eval_string(command);
		if (jl_exception_occurred())
	        SF_display("setting failed\n");
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_set_macros() {
	char *str = NULL;
	// Variable not required to be defined
	if( get_julia_string("set_macros", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char* content;
		char command[80];
		snprintf(command, 80, "getMacro(\"%s\")", name);
		jl_value_t* ret = jl_eval_string(command);
		if (jl_exception_occurred())
			SF_display("getting from Julia failed\n");
		content = jl_string_ptr(ret);
		SF_macro_save(name, content);
		// TODO: print error if not found in Stata
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_get_scalars() {
	char *str = NULL;
	int rc = 0;
	ST_double d;
	// Variable not required to be defined
	if( get_julia_string("get_scalars", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		if(rc = SF_scal_use(name, &d)) return(rc) ;   /* read scalar */
		jl_value_t *x = jl_box_float64(d);
		if( call_julia("addScalar", jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) return 321;
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_set_scalars() {
	char *str = NULL;
	int rc = 0;
	ST_double d = 0;
	// Variable not required to be defined
	if( get_julia_string("set_scalars", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		jl_value_t *x;
		// TODO: only floats work
		char command[80];
		snprintf(command, 80, "getScalar(\"%s\")", name);
		if( (x = jl_eval_string(command)) == NULL ) return 321;

		d = jl_unbox_float64(x);
		if(rc = SF_scal_save(name, d)) {
			SF_error("Could not save scalar.\n");
		}
        name = strtok(NULL, " "); // Get next name
    }
    return 0;
}

// Get Stata global array by name
jl_array_t *SJ_get_variable(char* name, int var_index) {
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
	if( (x = create_2D(rows, 1)) == NULL ) return 123;
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
jl_array_t *SJ_get_matrix(char* name) {
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
	if( (x = create_2D(rows, cols)) == NULL ) return 123;

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
int SJ_set_variable(char* name, int var_index, jl_array_t *x) {
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
int SJ_set_matrix(char* name, jl_array_t *x) {
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
		return 99;
	}

	// Get number of dimensions
	int ndims = jl_array_ndims(x);
	size_t rowsj = jl_array_dim(x, 0);
	size_t colsj = jl_array_dim(x, 1);
	if (rowsj != rows || colsj != cols) {
		snprintf(errbuf, 80, "ERROR: matrix dimensions for do not match for: %s", name);
		return 99;
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

// Execute single Julia command, returning the result in a Stata macro
int SJ_execute_command(char *command) {
	char buf[80] ;
	if (strlen(command) == 0) {
		SF_error("Either using och command options have to be set");
		return 1;
	}
	jl_value_t *ret = jl_eval_string(command);
	if (jl_exception_occurred()) {
		SF_error("Could not understand Julia command");
		return 1;
	}
	if (jl_typeis(ret, jl_float64_type)) {
			double ret_unboxed = jl_unbox_float64(ret);
			snprintf(buf, 80, "Result: %f\n", ret_unboxed) ;
			SF_display(buf);
			// TODO: Fix return values as global
			// SF_scal_save("r(command)", ret_unboxed);
			// SF_scal_save("command", ret_unboxed);
			return 0;
	} else {
		int ret_unboxed = jl_unbox_int64(ret);
		if (jl_exception_occurred()) {
			SF_error("Only expressions returning a float or double are allowed.\n");
			return 1;
		}
		snprintf(buf, 80, "Result: %d\n", ret_unboxed) ;
		SF_display(buf);
		return 0;
	}
}
