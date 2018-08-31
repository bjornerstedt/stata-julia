
#include "stplugin.h"
#include <julia.h>
JULIA_DEFINE_FAST_TLS();

STDLL stata_call(int argc, char *argv[]);
int process(char* array1, char* array2, char *command);
int set_array(char* name, jl_array_t *x);
jl_array_t *get_array(char* name);

STDLL stata_call(int argc, char *argv[])
{
	int retval;
	jl_init();
	jl_eval_string("include(\"init.jl\")");
	if (jl_exception_occurred()) {
		SF_error("File init.jl not found");
		 SF_error((char*)jl_typeof_str(jl_exception_occurred()));
		 return 101;
		}
	retval = process( "A", "B", "testf!");
	jl_atexit_hook(0);
	return(retval) ;
}

jl_array_t *get_array(char* name) {
	char errbuf[80] ;
	ST_int rows = SF_row(name);
	ST_int cols = SF_col(name);
	ST_int i, j;
	ST_retcode rc ;
	ST_double z, w;

	if (rows == 0) {
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

int set_array(char* name, jl_array_t *x) {
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
		snprintf(errbuf, 80, "ERROR: matrix dimensions for do not match for: ", name);
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

int process(char* array1, char* array2, char *command)
{
		jl_array_t *x = get_array(array1);
		if (x == NULL) {
			SF_error("Could not get array.\n");
			return 999;
		}

		// Process matrix in Julia
		jl_function_t *func = jl_get_function(jl_current_module, command);
		if (jl_exception_occurred()) {
			SF_error("Could not get function.\n");
			return 99;
		}
		jl_call1(func, (jl_value_t *)x);
		if (jl_exception_occurred()) {
			SF_error("Could not call function: ");
			SF_error(command);
			return 99;
		}

		int rc = set_array(array2, x);
		if (rc) {
			SF_error("Could not set array.\n");
			return rc;
		}
    return 0;
}


// Do something
// call_julia1(x, "testf!");
// if (jl_exception_occurred()) {
// 	SF_display("No way!");
// 	SF_display(jl_typeof_str(jl_exception_occurred()) );
// 	return 1;
// } else
