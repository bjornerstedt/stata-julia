
int main(int argc, char *argv[]);
STDLL stata_call(int argc, char *argv[]);
int SJ_process(char *funcname, char *varlist);
int SJ_set_matrix(char* name, jl_array_t *x, int is_variable);
jl_array_t *SJ_get_matrix(char* name);
jl_value_t *SJ_get_jl_var(char* name);
int SJ_set_jl_var(char* name, jl_value_t *x);
int SJ_execute_command(char *command);
int SJ_get_macros();
int SJ_set_macros();
int SJ_get_matrices();
int SJ_set_matrices();
int SJ_get_dataset();
int SJ_get_set_variables(int update);
int getIndices(char* list, char* list2, int tokenIndex[]);
int get_julia_var(char *varname, char** str);
