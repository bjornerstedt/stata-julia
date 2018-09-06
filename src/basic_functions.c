#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

// print Julia buffer
int displayPrintBuffer() {
    jl_value_t* ret;
    ret = jl_eval_string("getPrintBuffer()");
    if (jl_exception_occurred())
        SF_error("Failed getting the print Buffer from Julia\n");
    // jl_string_ptr(ret);
    SF_display(jl_string_ptr(ret));
    return 0;
}

// execute Julia command
int jexec(char *command) {
    jl_value_t* ret;
    ret = jl_eval_string(command);
    if (jl_exception_occurred()) {
        SF_error("Failed getting the print Buffer from Julia\n");
        return 22;
    }
    return 0;
}

// Get global string var from Julia
int get_julia_string(char *varname, char** str) {
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
        SF_display("call_julia: Calling Julia function failed\n");
		return NULL;
	}
	return rv;
}
