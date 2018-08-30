#include <julia.h>
JULIA_DEFINE_FAST_TLS();

int main(int argc, char *argv[]) {
    jl_init();
    //  Load init.jl if it exists.
    jl_eval_string("include(\"init.jl\")");
    if (jl_exception_occurred())
      printf("File init.jl not found, %s \n", jl_typeof_str(jl_exception_occurred()));
    int retval = communicate(argc, argv);
    // Get Julia string buffer and print
    jl_atexit_hook(0);
    return retval;
}

void printit(jl_value_t *ret) {
  if (jl_typeis(ret, jl_float64_type)) {
      double ret_unboxed = jl_unbox_float64(ret);
      printf("Value: %f \n", ret_unboxed);
  }
  else {
      printf("ERROR: unexpected return type from sqrt(::Float64)\n");
  }
}

jl_array_t *call_julia1(jl_array_t* x, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
  jl_call1(func, (jl_value_t*)x);
}

jl_array_t *call_julia2(jl_array_t* x, jl_array_t* y, char *funcname) {
  jl_function_t *func = jl_get_function(jl_current_module, funcname);
  jl_call2(func, (jl_value_t*)x, (jl_value_t*)y);
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

int communicate(int argc, char *argv[])
{
  jl_eval_string("print(f(\"test\"))");

  // jl_value_t *ret = jl_eval_string("sqrt(3.0)")

  jl_function_t *func = jl_get_function(jl_current_module, "g");
  jl_value_t *argument1 = jl_box_float64(8.0);
  jl_value_t *argument2 = jl_box_float64(6.0);
  jl_value_t *ret = jl_call2(func, argument1, argument2);
  printit(ret);

  // Multiple arguments
  jl_value_t **argument;
  jl_function_t *func2 = jl_get_function(jl_current_module, "h");
  JL_GC_PUSHARGS(argument, 4);
  argument[0] = jl_box_float64(8.0);
  argument[1] = jl_box_float64(11.0);
  argument[2] = jl_box_float64(5.0);
  argument[3] = jl_box_float64(7.0);
  jl_value_t *ret2 = jl_call(func2, argument, 4);
  printit(ret2);
  JL_GC_POP();

  create_vector();
  printf("\n" );
  create_array();
  printf("\n" );
  return 0;
}
