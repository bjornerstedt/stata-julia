
int main(int argc, char *argv[]);
STDLL stata_call(int argc, char *argv[]);
int SJ_process(char* array1, char* array2, char *command);
int SJ_set_matrix(char* name, jl_array_t *x);
jl_array_t *SJ_get_matrix(char* name);
jl_value_t *SJ_get_jl_var(char* name);
int SJ_set_jl_var(char* name, jl_value_t *x);
int SJ_execute_command(char *command);
int SJ_get_macros();
int SJ_set_macros();
