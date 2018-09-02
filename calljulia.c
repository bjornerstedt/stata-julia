#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

// Invoked by main funcname stata_call
int SJ_process(char *funcname)
{
		int rc = 0;
		SJ_get_macros();
		SJ_get_matrices();

		jl_function_t *func = jl_get_function(jl_current_module, funcname);
		if (jl_exception_occurred() || func == NULL) {
	        SF_error("function not found\n");
			return 1010;
		}
		jl_call0(func);
		SJ_set_matrices();
		SJ_set_macros();
		return rc;
}

int SJ_get_matrices() {
	jl_value_t *ret = jl_eval_string("get_matrices");
    if (jl_exception_occurred()) {
        SF_display("get_matrices not found\n");
		return 1;
	}
    char *str = jl_string_ptr(ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		jl_array_t *x = SJ_get_matrix(name);
		jl_function_t *func = jl_get_function(jl_current_module, "addMatrix");
		if (jl_exception_occurred() || func == NULL) {
	        SF_error("function addMatrix not found\n");
			return 1010;
		}
	  	jl_value_t *jname = jl_cstr_to_string(name);
	    jl_call2(func, jname, x);
		if (jl_exception_occurred())
	        SF_display("setting of matrix in Julia failed\n");
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_set_matrices() {
	jl_value_t *ret = jl_eval_string("set_matrices");
    if (jl_exception_occurred()) {
        SF_display("set_matrices not found\n");
		return 1;
	}
    char *str = jl_string_ptr(ret);
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
    jl_value_t *ret = jl_eval_string("get_macros");
    if (jl_exception_occurred()) {
        printf("get_macros not found, %s \n", jl_typeof_str(jl_exception_occurred()));
		return 1;
	}
    char *str = jl_string_ptr(ret);
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
    jl_value_t *ret = jl_eval_string("set_macros");
    if (jl_exception_occurred()) {
        SF_display("set_macros not found\n");
		return 1;
	}
    char *str = jl_string_ptr(ret);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char* content;
		char command[80];
		snprintf(command, 80, "getMacro(\"%s\")", name);
		ret = jl_eval_string(command);
		if (jl_exception_occurred())
			SF_display("getting failed\n");
		content = jl_string_ptr(ret);
		SF_macro_save(name, content);
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

// Get Stata global array by name
jl_array_t *SJ_get_matrix(char* name) {
	ST_int rows = SF_row(name);
	ST_int cols = SF_col(name);
	ST_int i, j;
	ST_retcode rc ;
	ST_double z, w;

	if (rows == 0) {
		char errbuf[80] ;
	  snprintf(errbuf, 80, "Could not get matrix: %s\n", name) ;
	  SF_display(errbuf);
		return NULL;
	}

	// Create 2D array of float64 type
    jl_value_t *array_type = jl_apply_array_type((jl_value_t*)jl_float64_type, 2);
	if (jl_exception_occurred()) {
		SF_error("Could not allocate memory 1.\n");
		return NULL;
	}

	jl_array_t *x  = jl_alloc_array_2d(array_type, rows, cols);
	if (jl_exception_occurred()) {
		SF_error("Could not allocate memory.\n");
		return NULL;
	}

	// Get array pointer
    double *xData = (double*)jl_array_data(x);

	for( i = 0; i < rows; i++) {
		for( j = 0; j < cols; j++) {
			// NOTE that Stata uses 1 based index!
			if((rc = SF_mat_el(name, i + 1, j + 1, &z))) return(rc) ;
			xData[j + cols*i] = z;
		}
	}
	return x;
}

int SJ_set_matrix(char* name, jl_array_t *x) {
	char errbuf[80] ;
	ST_int rows = SF_row(name);
	ST_int cols = SF_col(name);

	ST_int i, j;
	ST_retcode rc ;
	ST_double z, w;

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
			z = xData[j + cols * i];
			// NOTE that Stata uses 1 based index!
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
