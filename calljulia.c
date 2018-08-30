
#include "stplugin.h"
#include <julia.h>
JULIA_DEFINE_FAST_TLS();

int process(char* name, char *command);

STDLL stata_call(int argc, char *argv[])
{
	int retval;
	jl_init();
	jl_eval_string("include(\"init.jl\")");
	if (jl_exception_occurred()) {
		SF_error("File init.jl not found");
		 SF_error(jl_typeof_str(jl_exception_occurred()));
		 return 101;
		}
	retval = process( "A", "testf!");
	jl_atexit_hook(0);
	return(retval) ;
}

jl_array_t *get_array(char* name) {
	ST_int rows = SF_row(name);
	ST_int cols = SF_col(name);
	ST_int i, j;
	ST_retcode rc ;
	ST_double z, w;
	// Create 2D array of float64 type
  jl_value_t *array_type = jl_apply_array_type(jl_float64_type, 2);
	jl_array_t *x  = jl_alloc_array_2d(array_type, rows, cols);
	// Get array pointer
  double *xData = (double*)jl_array_data(x);

	for( i = 0; i < rows; i++) {
		for( j = 0; j < cols; j++) {
			// NOTE that Stata uses 1 based index!
			if((rc = SF_mat_el(name, i + 1, j + 1, &z))) return(rc) ;
			xData[j + rows*i] = z;
		}
	}
	return x;
}

int set_array(char* name, jl_array_t *x) {
	ST_int rows = SF_row(name);
	ST_int cols = SF_col(name);
	ST_int i, j;
	ST_retcode rc ;
	ST_double z, w;

	// Get number of dimensions
	int ndims = jl_array_ndims(x);
	// Get the size of the i-th dim
	size_t size0 = jl_array_dim(x, 0);
	size_t size1 = jl_array_dim(x, 1);

	// Get array pointer
  double *xData = (double*)jl_array_data(x);

	for( i = 0; i < rows; i++) {
		for( j = 0; j < cols; j++) {
			z = xData[j + rows * i];
			// NOTE that Stata uses 1 based index!
			if((rc = SF_mat_store("B", i + 1, j + 1, z))) return(rc) ;
		}
	}
	return 0;
}

jl_array_t *call_julia1(jl_array_t* x, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
	if (!jl_exception_occurred()) {
  	(jl_array_t*)jl_call1(func, (jl_value_t*)x);
	}
}

int process(char* name, char *command)
{
		jl_array_t *x = get_array(name);

		// Process matrix in Julia
		jl_function_t *func = jl_get_function(jl_current_module, command);
		if (!jl_exception_occurred()) {
	  	jl_call1(func, x);
		}

		set_array(name, x);

    return 0;
}


// Do something
// call_julia1(x, "testf!");
// if (jl_exception_occurred()) {
// 	SF_display("No way!");
// 	SF_display(jl_typeof_str(jl_exception_occurred()) );
// 	return 1;
// } else
