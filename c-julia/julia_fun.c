#include <julia.h>
#include "julia_fun.h"
#include <string.h>

JULIA_DEFINE_FAST_TLS();

int main(int argc, char *argv[]) {
    jl_init();
    //  Load init.jl if it exists.
    jl_eval_string("include(\"inittest.jl\")");
    if (jl_exception_occurred())
      printf("File init.jl not found, %s \n", jl_typeof_str(jl_exception_occurred()));
    int retval = communicate(argc, argv);
    // Get Julia string buffer and print
    jl_atexit_hook(0);
    return retval;
}

int communicate(int argc, char *argv[]) {
  jl_value_t *ret3 = jl_eval_string("MyModule.f()");
  if (jl_exception_occurred() || ret3 == NULL)
    printf("File init.jl not found, %s \n", jl_typeof_str(jl_exception_occurred()));
  else
    printit(ret3);
{
  // jl_value_t *ret = jl_eval_string("sqrt(3.0)")
jl_value_t *xx = jl_eval_string("MyModule");
jl_function_t *func = jl_get_function(xx, "f");
// jl_value_t *argument = jl_box_float64(2.0);
// jl_value_t *ret = jl_call1(func, argument);
jl_value_t *ret = jl_call0(func);
if (jl_exception_occurred() || ret == NULL)
  printf("dasfanot found, %s \n", jl_typeof_str(jl_exception_occurred()));
else
  printit(ret);
}
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
  // Array operations
  // create_vector();
  // printf("\n" );
  create_array();
  // printf("\n" );
  return 0;
}
