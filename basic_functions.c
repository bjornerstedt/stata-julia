#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

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
    tokenIndex[i] = 0;
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
