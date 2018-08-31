#include "stplugin.h"
#include <julia.h>
#include <strings.h>
JULIA_DEFINE_FAST_TLS();

STDLL stata_call(int argc, char *argv[]);
int process(char* array1, char* array2, char *command);
int set_array(char* name, jl_array_t *x);
jl_array_t *get_array(char* name);
int execute_command(char *command);

STDLL stata_call(int argc, char *argv[])
{
	jl_init();
	if (argc != 3) {
		SF_error("Internal error. ADO file has sent the wrong number of pars");
		return 1;
	}
	char* method = argv[0];
	char* using = argv[1];
	char* command = argv[2];
	char buf[80] ;

	if (strlen(using) == 0) {
		return execute_command(command);
	}
	if (strlen(method) == 0) {
		SF_error("Either method or command has to be specified.\n");
		return 1;
	}

	int retval = 0;
	snprintf(buf, 80, "include(\"%s\")", using) ;
	jl_eval_string(buf);
	if (jl_exception_occurred()) {
		SF_error("File init.jl not found");
		 SF_error((char*)jl_typeof_str(jl_exception_occurred()));
		 return 101;
	}

	// Get matrix names from init.jl
	jl_value_t *ret = jl_eval_string("inmatrix");
	if (jl_exception_occurred()) {
		SF_error("inmatrix not found");
		 return 101;
	}
	char *mat1 = jl_string_ptr(ret);
	ret = jl_eval_string("outmatrix");
	if (jl_exception_occurred()) {
		SF_error("outmatrix not found");
		 return 101;
	}
	char *mat2 = jl_string_ptr(ret);

	// Invoke command
	retval = process( mat1, mat2, "testf!");

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

int execute_command(char *command) {
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
		snprintf(buf, 80, "Result: %ld\n", ret_unboxed) ;
		SF_display(buf);
		return 0;
	}
}
