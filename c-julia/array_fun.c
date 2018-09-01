#include <julia.h>
#include "julia_fun.h"

// Print Julia double
void printit(jl_value_t *ret) {
  if (jl_typeis(ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(ret);
      printf("Value: %f \n", ret_unboxed);
  }
  else {
      printf("ERROR: unexpected return type from sqrt(::Float64)\n");
  }
}

jl_value_t *call_julia1(jl_array_t* x, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
  return jl_call1(func, (jl_value_t*)x);
}

jl_value_t *call_julia2(jl_array_t* x, jl_array_t* y, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
  return jl_call2(func, (jl_value_t*)x, (jl_value_t*)y);
}

void create_vector() {
  // Create vector and reverse it
  jl_value_t* array_type = jl_apply_array_type((jl_value_t*)jl_float64_type, 1);
  jl_array_t* x  = jl_alloc_array_1d(array_type, 10);
  // xData is the C data representation
  double *xData = (double*)jl_array_data(x);
  for(size_t i=0; i<jl_array_len(x); i++)
    xData[i] = i;
  call_julia1(x, "rev");
  if (jl_exception_occurred())
      printf("%s \n", jl_typeof_str(jl_exception_occurred()));
  for(size_t i=0; i<jl_array_len(x); i++)
    printf("%f\n", xData[i] );
}

void print_array(jl_array_t *x) {
  double *xData = (double*)jl_array_data(x);
  size_t size0 = jl_array_dim(x,0);
  size_t size1 = jl_array_dim(x,1);
  for(size_t i=0; i<size1; i++) {
      for(size_t j=0; j<size0; j++)
        printf("%f, ", xData[j + size0*i] );
      printf("\n" );
  }
}

// Create example array and send to
void create_array() {
  // Create 2D array of float64 type
  jl_value_t *array_type = jl_apply_array_type(jl_float64_type, 2);
  jl_array_t *x  = jl_alloc_array_2d(array_type, 10, 5);

  // Get array pointer
  double *xData = (double*)jl_array_data(x);
  // Get number of dimensions
  int ndims = jl_array_ndims(x);
  // Get the size of the i-th dim
  size_t size0 = jl_array_dim(x,0);
  size_t size1 = jl_array_dim(x,1);

  // Fill array with data
  for(size_t i=0; i<size1; i++)
      for(size_t j=0; j<size0; j++)
          xData[j + size0*i] = i + j;
  // Do something
  call_julia1(x, "testf!");
  if (jl_exception_occurred())
    printf("%s \n", jl_typeof_str(jl_exception_occurred()));
  print_array(x);
}
