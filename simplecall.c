
#include "stplugin.h"
#include <julia.h>
JULIA_DEFINE_FAST_TLS();

STDLL stata_call(int argc, char *argv[])
{
	jl_init();

	jl_value_t *ret = jl_eval_string(argv[0]);
	if (jl_typeis(ret, jl_float64_type)) {
			double ret_unboxed = jl_unbox_float64(ret);
			SF_scal_save("x", ret_unboxed);
	}
	else {
		SF_scal_save("x", -99.0);
	}

	jl_atexit_hook(0);
	return(0) ;
}
