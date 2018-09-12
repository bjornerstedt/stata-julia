// C application entry point
int main2(int argc, char *argv[]) {
	int retval = 0;
	char buf[80] ;

	if (argc < 2) {
		// if (argc != 12) {
		SF_error("Internal error. The ADO file has sent the wrong number of pars");
		return 1;
	}
	char* function = argv[0];
	char* using = argv[1];
	char* command = (2 < argc)?argv[2]:NULL;

	// Run file in 'using' if it has been specified
	static int invoked = 0;
	char* save = NULL;
	if (!invoked) {
		if (strlen(using)) {
			snprintf(buf, 80, "include(\"%s\")", using) ;
			jl_eval_string(buf);
			if (jl_exception_occurred()) {
				snprintf(buf, 80, "File %s not executed, error: %s.\n", using, jl_typeof_str(jl_exception_occurred())) ;
				SF_error(buf);
				return 101;
			}
		}
		// jl_value_t *stata = jl_eval_string("StataJulia.getInstance()");
		// if (stata == NULL || jl_exception_occurred()) {
		// 	return 1999;
		// }

		int i = 3;
		if (i < argc) {
			save = argv[i];
			julia_set_varlist("savefile", argv[i++]);
		}
		julia_set_varlist("get_variables", (i < argc)?argv[i++]:"");
		julia_set_varlist("set_variables", (i < argc)?argv[i++]:"");
		julia_set_varlist("get_matrices", (i < argc)?argv[i++]:"");
		julia_set_varlist("set_matrices", (i < argc)?argv[i++]:"");
		julia_set_varlist("get_scalars", (i < argc)?argv[i++]:"");
		julia_set_varlist("set_scalars", (i < argc)?argv[i++]:"");
		julia_set_varlist("get_macros", (i < argc)?argv[i++]:"");
		julia_set_varlist("set_macros", (i < argc)?argv[i++]:"");
	} else {
		SF_display("Repeated invocation!\n");
	}

	// Stata has if or in
	// TODO: remove has_selection, as touse is always created
	// int has_selection = strlen(argv[12]);
	if (save != NULL ) {
		if ( strlen(save) ) {
			snprintf(buf, 80, "Saving data to file: %s\n", save) ;
			SF_display(buf);
			function = "serializeData";
		}
	}
	if (command != NULL && strlen(command)) {
		return execute_command(command);
	}
	if (function != NULL && strlen(function) == 0) {
		SF_error("Either function or command has to be specified.\n");
		return 211;
	}
	// Invoke command
	retval = process( function, invoked++);

	// jexec("StataJulia.destructor()");
	return(retval) ;
}
