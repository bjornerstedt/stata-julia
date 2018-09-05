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

		jl_function_t *func = jl_get_function(jl_current_module, funcname);
		if (jl_exception_occurred() || func == NULL) {
	        SF_error("function not found\n");
			return 1010;
		}
		jl_call0(func);
		SJ_set_matrices();
		SJ_set_macros();
		SJ_set_scalars();
		// SJ_get_set_variables(1);
		return rc;
}

int SJ_get_matrices() {
	char *str = NULL;
	// Variable not required to be defined
	if( get_julia_var("get_matrices", &str) )
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
	if( get_julia_var("set_matrices", &str) )
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
		SJ_set_matrix(name, (jl_array_t *)x, 0);
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

// This function is called to get and set variables that
// can be modified in Julia
int SJ_get_set_variables(int update) {
	char *vars = NULL;
	char *setvars = NULL;
	// Variable not required to be defined
	if( get_julia_var("get_variables", &vars) )
		return 0;
	if( get_julia_var("set_variables", &setvars) )
		return 0;

    char *name = strtok(setvars, " ");
    while( name != NULL ) {
		char command[80];
		snprintf(command, 80, "getVariable(\"%s\")", name);
		SF_display(command);
		jl_value_t *x = jl_eval_string(command);
	  	if (jl_exception_occurred()) {
	  		SF_error("Could not get variable from Julia\n");
	  		return 1;
	  	}
		SJ_set_matrix(name, (jl_array_t *)x, 0);
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_get_macros() {
	char *str = NULL;
	// Variable not required to be defined
	if( get_julia_var("get_macros", &str) )
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
	if( get_julia_var("set_macros", &str) )
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
	if( get_julia_var("get_scalars", &str) )
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
	if( get_julia_var("set_scalars", &str) )
		return 0;
    char *name = strtok(str, " ");
    while( name != NULL ) {
		jl_value_t *x;
		// TODO: only floats work
		char command[80];
		snprintf(command, 80, "getScalar(\"%s\")", name);
		if( (x = jl_eval_string(command)) == NULL ) return 321;

		d = jl_unbox_float64(x);
		// int e = jl_unbox_int64(x);
		//
		{
			char errbuf[80] ;
			snprintf(errbuf, 80, "Scalar: %d\n", d) ;
			SF_display(errbuf);

		}
		if(rc = SF_scal_save(name, d)) {
			SF_error("Could not save scalar.\n");
		}
        name = strtok(NULL, " "); // Get next name
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
	if( (x = create_2D(rows, cols)) == NULL ) return 123;
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

// Create 2D array of float64 type
jl_array_t* create_2D(int rows, int cols) {
    jl_value_t *array_type = jl_apply_array_type((jl_value_t*)jl_float64_type, 2);
	if (jl_exception_occurred() || array_type == NULL) {
		SF_error("Could not allocate memory 1.\n");
		return NULL;
	}
	jl_array_t *x  = jl_alloc_array_2d(array_type, rows, cols);
	if (jl_exception_occurred() || x == NULL) {
		SF_error("Could not allocate memory.\n");
		return NULL;
	}
	return x;
}

jl_value_t *call_julia(char *funcname, jl_value_t* x, jl_value_t* y) {
	jl_function_t *func = jl_get_function(jl_current_module, funcname);
	if (jl_exception_occurred() || func == NULL) {
		char errbuf[80] ;
		snprintf(errbuf, 80, "Function not found: %s\n", funcname) ;
		SF_display(errbuf);
		return NULL;
	}
	jl_value_t *rv;
	if (x == NULL) {
		rv = jl_call0(func);
	} else if (y == NULL) {
		rv = jl_call1(func, x);
	} else {
		rv = jl_call2(func, x, y);
	}
	if (jl_exception_occurred() || rv == NULL) {
		SF_display("Calling Julia function failed\n");
		return NULL;
	}
	return rv;
}



// Get Stata global array by name
jl_array_t *SJ_get_matrix(char* name) {
	ST_int rows = SF_row(name);
	ST_int cols = SF_col(name);
	ST_int i, j;
	ST_retcode rc ;

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
int SJ_set_matrix(char* name, jl_array_t *x, int var_index) {
	char errbuf[80] ;
	ST_int rows, cols, min_cols;
	if (var_index) {
		rows = SF_in2() ;
		 // Only set column var_index if it is a variable
		cols = var_index;
		min_cols = var_index - 1;
	} else {
		min_cols = 0;
		rows = SF_row(name);
		cols = SF_col(name);
	}

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
		for( j = min_cols; j < cols; j++) {
			z = xData[j + cols * i];
			// NOTE that Stata uses 1 based index!
			if (var_index) {
				if((rc = SF_vstore(var_index, j + 1, z))) return(rc) ;

			} else {
				if((rc = SF_mat_store(name, i + 1, j + 1, z))) return(rc) ;
			}
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

// Get token positions of tokens in list2 in list and return in tokenIndex
int getIndices(char* list, char* list2, int tokenIndex[]) {
    char* token[80];
    int i = 0;
    char *name = strtok(list, " ");
    while( name != NULL ) {
        char command[80];
        token[i++] = name;
        name = strtok(NULL, " ");
    }
    int token1count = i;

    char* token2[80];
    i = 0;
    name = strtok(list2, " ");
    while( name != NULL ) {
        char command[80];
        token2[i++] = name;
        name = strtok(NULL, " ");
    }
    int token2count = i;

    name = strtok(list, " ");
    for (i = 0; i < token2count; i++) {
        for (size_t j = 0; j < token1count; j++) {
            if (!strcmp(token2[i], token[j]) ) {
                tokenIndex[i] = j + 1; // Stata is 1 based
            }
        }
    }
    return 0;
}

// Get global string var from Julia
int get_julia_var(char *varname, char** str) {
	jl_value_t *ret = jl_eval_string(varname);
	if (jl_exception_occurred()) {
		char command[80];
		snprintf(command, 80, "Could not get Julia var: %s\n", varname);
		SF_display(command);
		return 1;
	}
	*str = jl_string_ptr(ret);
	return 0;
}
