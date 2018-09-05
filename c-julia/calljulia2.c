#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

// Invoked by main funcname stata_call
int SJ_process(char *funcname, char *varlist)
{
		int rc = 0;
		SJ_get_macros_scalars();
		SJ_get_matrices();
		SJ_get_dataset();

		jl_function_t *func = jl_get_function(jl_current_module, funcname);
		if (jl_exception_occurred() || func == NULL) {
	        SF_error("function not found\n");
			return 1010;
		}
		jl_call0(func);
		SJ_set_matrices();
		SJ_set_macros_scalars();
		SJ_set_dataset();
		return rc;
}

int SJ_get_matrices() {
	char *str = NULL;
	get_julia_var("get_matrices", &str);
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
	get_julia_var("set_matrices", &str);

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

int SJ_set_dataset() {
	// Get array of strings from Julia, where strings are empty (or perhaps NULL)
	// unless it is a variable to write.
	char *str = NULL;
	get_julia_var("set_variables", &str);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char command[80];
		snprintf(command, 80, "getVariable(\"%s\")", name);
		SF_display(command);
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

int SJ_get_macros_scalars() {
	char juliaGlobal[2];
	juliaGlobal[0] = "get_macros";
	juliaGlobal[1] = "get_scalars";
	for (size_t i = 0; i < 2; i++) {
		char *str = NULL;
		get_julia_var("get_macros", &str);
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
	}
    return 0;
}

int SJ_set_macros_scalars() {
	char *str = NULL;
	get_julia_var("set_macros", &str);
    char *name = strtok(str, " ");
    while( name != NULL ) {
		char* content;
		char command[80];
		snprintf(command, 80, "getMacro(\"%s\")", name);
		jl_value_t* ret = jl_eval_string(command);
		if (jl_exception_occurred())
			SF_display("getting failed\n");
		content = jl_string_ptr(ret);
		SF_macro_save(name, content);
		// Get next name
        name = strtok(NULL, " ");
    }
    return 0;
}

int SJ_get_dataset() {
	ST_int cols = SF_nvars();
	if (cols == 0) {
		// It is OK to not have a dataset
		return 0;
	}
	ST_int rows = SF_in2() ;

	// the Dict with variables to be set has to be created
	char *str = NULL;
	get_julia_var("set_variables", &str);

	char *name = strtok(str, " ");
    while( name != NULL ) {
		char* content;
		char command[80];
		snprintf(command, 80, "addVariable(\"%s\")", name);
		jl_value_t* ret = jl_eval_string(command);
		if (jl_exception_occurred())
			SF_display("getting failed\n");
		content = jl_string_ptr(ret);
		SF_macro_save(name, content);
		// Get next name
        name = strtok(NULL, " ");
    }


	// Create 2D array of float64 type
    jl_value_t *array_type = jl_apply_array_type((jl_value_t*)jl_float64_type, 2);
	if (jl_exception_occurred()) {
		SF_error("Could not allocate memory 1.\n");
		return 99;
	}
	jl_array_t *x  = jl_alloc_array_2d(array_type, rows, cols);
	if (jl_exception_occurred() || x == NULL) {
		SF_error("Could not allocate memory.\n");
		return 99;
	}

	// Copy data from Stata to Julia
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

	// Put data in Julia global Dict
	jl_function_t *func = jl_get_function(jl_current_module, "addDataset");
	if (jl_exception_occurred() || func == NULL) {
		SF_error("function addDataset not found\n");
		return 1010;
	}
	jl_call1(func, (jl_value_t *)x);
	if (jl_exception_occurred())
		SF_display("setting of dataset in Julia failed\n");
    return 0;
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
