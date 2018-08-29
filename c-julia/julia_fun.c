#include <julia.h>
JULIA_DEFINE_FAST_TLS();

int main(int argc, char *argv[])
{
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
  JL_GC_PUSHARGS(argument, 4); // argument can now hold 2 `jl_value_t*` objects
  argument[0] = jl_box_float64(8.0);
  argument[1] = jl_box_float64(11.0);
  argument[2] = jl_box_float64(5.0);
  argument[3] = jl_box_float64(7.0);
  jl_value_t *ret2 = jl_call(func2, argument, 4);
  printit(ret2);
  JL_GC_POP();

  printf("\n" );
  return 0;
}
