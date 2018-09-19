
int main(int argc, char *argv[]);
STDLL stata_call(int argc, char *argv[]);
int process(char *funcname, int ivoked);
int set_matrix(char* name, jl_array_t *x);
int set_variable(char* name, int var_index, jl_array_t *x);
jl_array_t *get_matrix(char* name);
jl_array_t *get_variable(char* name, int var_index);
jl_value_t *get_jl_var(char* name);
int set_jl_var(char* name, jl_value_t *x);
int execute_command(char *command);
int variables(int has_selection);
int set_variables(int first_time);

int set_matrices(jl_value_t *stata, jl_value_t *stata_data);
int matrices(jl_value_t *stata, jl_value_t *stata_data);

int macros();
int set_macros();
int scalars();
int set_scalars();
int getIndices(char* list, char* list2, int tokenIndex[]);
int get_julia_string(char *command, char** str);
jl_value_t *call_julia(char *module, char *funcname, jl_value_t* x, jl_value_t* y, jl_value_t* z);
jl_array_t* create_2D(int rows, int cols);
int displayPrintBuffer();
int jexec(char *command);
int julia_set_varlist(char* name, char* varlist);
int create_selection();
