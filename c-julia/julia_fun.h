
int communicate(int argc, char *argv[]);
void printit(jl_value_t *ret);
void create_array();
void create_vector();
void print_array(jl_array_t *x);
jl_value_t *call_julia1(jl_array_t* x, char *funcname);
jl_value_t *call_julia2(jl_array_t* x, jl_array_t* y, char *funcname);
char* SJ_get_macros();
