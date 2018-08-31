
#include "stplugin.h"
#include <julia.h>
JULIA_DEFINE_FAST_TLS();

void print_error(char* text, char* errortext) {
	char buf[80];
  sprintf(buf, text, errortext);
	SF_display(buf);
}

void printit(jl_value_t *ret) {
  if (jl_typeis(ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(ret);
			char buf[80];
      sprintf(buf, "Value: %f \n", ret_unboxed);
			SF_display(buf);
  }
  else {
      SF_display("ERROR: unexpected return type from function\n");
  }
}

void print_double( double x) {
	char buf[80];
	sprintf(buf,  "%8.2f, ", x);
	SF_display(buf );
}

void print_array(jl_array_t *x) {
  double *xData = (double*)jl_array_data(x);
  size_t size0 = jl_array_dim(x, 0);
  size_t size1 = jl_array_dim(x, 1);
	{
		char buf[80];
		sprintf(buf,  "Dim: %d %d\n", size0, size1);
		SF_display(buf );
	}
	// for(size_t j = 0; j < size0; j++) {
  // 	for(size_t i = 0; i < size1; i++)
  //       print_double(xData[j + size0*i] );
  //     SF_display("\n" );
  // }
}

jl_array_t *call_julia0(char *funcname) {
	jl_array_t* y;

  jl_function_t *func = jl_get_function(jl_current_module, funcname);
	if (jl_exception_occurred())
		print_error("Function name problem, %s \n", jl_typeof_str(jl_exception_occurred()));
	else
		y = (jl_array_t*)jl_call0(func);
}

STDLL stata_call(int argc, char *argv[])
{
	jl_init();
	char* command = argv[0];

	//  Load init.jl if it exists.
	jl_eval_string("include(\"init.jl\")");
	if (jl_exception_occurred())
		print_error("File init.jl not found, %s \n", jl_typeof_str(jl_exception_occurred()));
		// int retval = communicate(command);
		int retval = communicate3(8,5);


	// jl_array_t *y;
	// y = (jl_array_t *)jl_eval_string("[1 2; 3 4; 5, 6]");
	// y = call_julia0("rowcol");
	// if (jl_exception_occurred()) {
	// 	print_error("Function calling problem, %s \n", jl_typeof_str(jl_exception_occurred()));
	// 	return 1;
	// }
	// char buf[80];
	// sprintf(buf, "Dim 0: %f \n", jl_array_dim(y, 0));
	// SF_display(buf);

	// Get Julia string buffer and print
	jl_atexit_hook(0);
	return 0;
}

jl_array_t *call_julia1(jl_array_t* x, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
	if (jl_exception_occurred()) {
		SF_error("Could not get function.\n");
		return NULL;
	} else {
  	(jl_array_t*)jl_call1(func, (jl_value_t*)x);
	}
}

jl_array_t *call_julia2(jl_array_t* x, jl_array_t* y, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
  (jl_array_t*)jl_call2(func, (jl_value_t*)x, (jl_value_t*)y);
}


// Safely get array from Stata to Julia format
jl_array_t *get_array(char* name) {
	ST_int rows = SF_row("A");
	ST_int cols = SF_col("A");
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
			if((rc = SF_mat_el("A", i + 1, j + 1, &z))) return(rc) ;
			xData[j + rows*i] = z;
		}
	}
	return NULL;
}

// Safely put Julia array in Stata matrix
int set_array(char* name, jl_array_t *x) {
	ST_int rows = SF_row("A");
	ST_int cols = SF_col("A");
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
			if((rc = SF_mat_store("B", i + 1, j + 1, 2*z))) return(rc) ;
		}
	}

}

int communicate3(int rows, int cols) {
	jl_array_t *x;
	x = get_array("A");
	if (x==NULL) {
		SF_display("ÄMÖ" );
	}
	{
		size_t size0 = jl_array_dim(x, 0);
		size_t size1 = jl_array_dim(x, 1);
		char buf[80];
		sprintf(buf,  "Dim here: %d %d\n", size0, size1);
		SF_display(buf );
	}

	// print_array(x);
	return 0;
}

int communicate2(int rows, int cols) {
  // Create 2D array of float64 type
  jl_value_t *array_type = jl_apply_array_type(jl_float64_type, 2);
  jl_array_t *x  = jl_alloc_array_2d(array_type, rows, cols);

  // Get array pointer
  double *xData = (double*)jl_array_data(x);
  // Get number of dimensions
  int ndims = jl_array_ndims(x);
  // Get the size of the i-th dim
  size_t size0 = jl_array_dim(x, 0);
  size_t size1 = jl_array_dim(x, 1);
//
  // Fill array with data
  for(size_t i=0; i<size1; i++)
      for(size_t j=0; j<size0; j++)
          xData[j + size0*i] = i + j;
  // Do something
  // call_julia1(x, "testf!");
  // if (jl_exception_occurred())
  //   print_error("%s \n", jl_typeof_str(jl_exception_occurred()));
	print_array(x);
	return 0;
}

int communicate(char *command)
{
	// TODO: Check for error
	// jl_value_t *ret = jl_eval_string(command);
	// if (jl_exception_occurred())
	// 	print_error("Command ERROR, %s \n", jl_typeof_str(jl_exception_occurred()));
	// printit(ret);

  jl_function_t *func = jl_get_function(jl_current_module, command);
	if (jl_exception_occurred()) {
		print_error("File init.jl not found, %s \n", jl_typeof_str(jl_exception_occurred()));
		return 1;
	}
	jl_value_t *argument1 = jl_box_float64(8.0);
  jl_value_t *argument2 = jl_box_float64(6.0);
  jl_value_t *ret = jl_call2(func, argument1, argument2);
  printit(ret);

  return 0;
}

// Do something
// call_julia1(x, "testf!");
// if (jl_exception_occurred()) {
// 	SF_display("No way!");
// 	SF_display(jl_typeof_str(jl_exception_occurred()) );
// 	return 1;
// } else
