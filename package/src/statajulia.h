
int main(int argc, char *argv[]);
STDLL stata_call(int argc, char *argv[]);
int process(char *using, char *funcname, jl_value_t *stata,  jl_value_t *stata_data);

jl_value_t *checked_eval_string(const char* code);
jl_value_t *get_jl_var(char* name);
int set_jl_var(char* name, jl_value_t *x);
int execute_command(char *command);

int variables(jl_value_t *stata, jl_value_t *stata_data, int update);
int matrices(jl_value_t *stata, jl_value_t *stata_data, int update);
int macros(jl_value_t *stata, jl_value_t *stata_data, int update);
int scalars(jl_value_t *stata, jl_value_t *stata_data, int update);

int get_variable(jl_value_t *stata, char* name, int var_index);
int set_variable(jl_value_t *stata, char* name, int var_index);
int matrix(jl_value_t *stata, char* name, int update);

int getIndices(char* list, char* list2, int tokenIndex[]);
int get_julia_string(char *command, char** str);
jl_value_t *call_julia(char *module, char *funcname, jl_value_t* x, jl_value_t* y, jl_value_t* z);
jl_array_t* create_1D(int rows);
jl_array_t* create_2D(int rows, int cols);
int displayPrintBuffer();
int jexec(char *command);
int julia_set_varlist(jl_value_t *stata_data, char* name, char* varlist);
int create_selection(jl_value_t *stata);
char* getNameFromList(jl_value_t *stata_data, char* namelist, int update, int index);
