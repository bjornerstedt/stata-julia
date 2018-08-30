
#include "stplugin.h"
#include <julia.h>
JULIA_DEFINE_FAST_TLS();

STDLL stata_call(int argc, char *argv[])
{
	jl_init();

	jl_value_t *ret = jl_eval_string(argv[0]);
	jl_value_t* array_type = jl_apply_array_type(jl_float64_type, 1);
	jl_array_t *y = (jl_array_t *)jl_eval_string("[1 2; 3 4; 5, 6]");

	jl_atexit_hook(0);
	return(0) ;
}

// Return scalar to Stata
// if (jl_typeis(ret, jl_float64_type)) {
// 		double ret_unboxed = jl_unbox_float64(ret);
// 		SF_scal_save("x", ret_unboxed);
// }
// else {
// 	SF_scal_save("x", -99.0);
// }
