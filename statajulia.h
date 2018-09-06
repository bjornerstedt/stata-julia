
int main(int argc, char *argv[]);
STDLL stata_call(int argc, char *argv[]);
int SJ_process(char *funcname, char *varlist);
int SJ_set_matrix(char* name, jl_array_t *x);
int SJ_set_variable(char* name, int var_index, jl_array_t *x);
jl_array_t *SJ_get_matrix(char* name);
jl_array_t *SJ_get_variable(char* name, int var_index);
jl_value_t *SJ_get_jl_var(char* name);
int SJ_set_jl_var(char* name, jl_value_t *x);
int SJ_execute_command(char *command);
int SJ_get_macros();
int SJ_set_macros();
int SJ_get_matrices();
int SJ_set_matrices();
int SJ_get_dataset();
int SJ_get_variables();
int SJ_set_variables();
int getIndices(char* list, char* list2, int tokenIndex[]);
int get_julia_string(char *command, char** str);
jl_value_t *call_julia(char *funcname, jl_value_t* x, jl_value_t* y);
jl_array_t* create_2D(int rows, int cols);
int SJ_get_scalars();
int SJ_set_scalars();
int displayPrintBuffer();
int jexec(char *command);
