#include <julia.h>
JULIA_DEFINE_FAST_TLS(); 

int main(int argc, char *argv[])
{
    jl_init();

    /* run Julia commands */
    jl_eval_string("x=2");
    printf("\n" );
    jl_eval_string("print(sqrt(x))");

    jl_value_t *ret = jl_eval_string("sqrt(3.0)");
    if (jl_typeis(ret, jl_float64_type)) {
        double ret_unboxed = jl_unbox_float64(ret);
        printf("sqrt(3.0) in C: %e \n", ret_unboxed);
    }
    else {
        printf("ERROR: unexpected return type from sqrt(::Float64)\n");
    }
    printf("\n" );

    jl_atexit_hook(0);
    return 0;
}
