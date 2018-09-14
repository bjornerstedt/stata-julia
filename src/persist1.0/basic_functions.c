#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"



// execute Julia command
int jexec(char *command) {
    jl_value_t* ret;
    ret = jl_eval_string(command);
    if (jl_exception_occurred()) {
        char buf[80];
    	snprintf(buf, 80, "Failed Executing Julia command: %s\n", command);
        SF_error(buf);
        return 22;
    }
    return 0;
}

// Get global string var from Julia
int get_julia_string(char *varname, char** str) {
	jl_value_t *ret = jl_eval_string(varname);
    if (ret == NULL || jl_exception_occurred()) {
        // Return silently, as not finding string is possible
        char command[80];
        snprintf(command, 80, "Could not get Julia String: %s %s\n", varname, jl_typeof_str(jl_exception_occurred()));
        SF_display(command);
		return 1;
	}
    // TODO: Use const char* declarations
	*str = (char *)jl_string_ptr(ret);
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

jl_value_t *call_julia(char *module, char *funcname, jl_value_t* x, jl_value_t* y, jl_value_t* z) {
    jl_value_t *my_module;
    jl_function_t *func;
    char errbuf[80] ;
    if (module == NULL) {
        func = jl_get_function(jl_current_module, funcname);
    } else {
        my_module = jl_eval_string(module);
        func = jl_get_function(my_module, funcname);
    }
	if (jl_exception_occurred() || func == NULL) {
		snprintf(errbuf, 80, "Function not found: %s\n", funcname) ;
		SF_error(errbuf);
		return NULL;
	}
	jl_value_t *rv;
	if (x == NULL) {
		rv = jl_call0(func);
	} else if (y == NULL) {
		rv = jl_call1(func, x);
	} else if (z == NULL) {
		rv = jl_call2(func, x, y);
	} else {
		rv = jl_call3(func, x, y, z);
	}
	if (jl_exception_occurred() || rv == NULL) {
        snprintf(errbuf, 80, "call_julia: %s, Error: %s\n", funcname, jl_typeof_str(jl_exception_occurred())) ;
		SF_error(errbuf);
		return NULL;
	}
	return rv;
}
