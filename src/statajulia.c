#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

JULIA_DEFINE_FAST_TLS();

// Stata plugin entry point
STDLL stata_call(int argc, char *argv[])
{
	return main(argc, argv);
}

// C application entry point
int main(int argc, char *argv[])
	{
	jl_init();
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
	jl_atexit_hook(0);
	return(retval) ;
}

// Execute single Julia command, returning the result in a Stata macro
// NOTE that it does not handla return values well, and does not use init values.
int execute_command(char *command) {
	char buf[80];
	if (strlen(command) == 0) {
		SF_error("Either using och command options have to be set");
		return 1456;
	}

	// Evaluate command:
	jl_value_t *ret = jl_eval_string(command);
	if (jl_exception_occurred()) {
		SF_error("Could not understand Julia command\n");
		return 1534;
	}
	if (jl_typeis(ret, jl_float64_type)) {
			double ret_unboxed = jl_unbox_float64(ret);
			snprintf(buf, 80, "Result: %f\n", ret_unboxed) ;
			SF_display(buf);
			// TODO: Fix return values as global
			// SF_scal_save("r(command)", ret_unboxed);
			// SF_scal_save("command", ret_unboxed);
			return 0;
	} else {
		int ret_unboxed = jl_unbox_int64(ret);
		if (jl_exception_occurred()) {
			SF_error("Only expressions returning a float or double are allowed.\n");
			return 1;
		}
		snprintf(buf, 80, "Result: %d\n", ret_unboxed) ;
		SF_display(buf);
		return 0;
	}
}

int julia_set_varlist(char* name, char* varlist) {
	if (varlist == NULL || !strlen(varlist)) {
		return 1;
	}
	char command[200];
	snprintf(command, 80, "StataJulia.addJuliaInitString(\"%s\", \"%s\")", name, varlist);
	printf("%s\n", command );
	int ret = jl_eval_string(command);
	if (jl_exception_occurred()) {
		SF_error("Setting init list in Julia failed\n");
		return 3293;
	}
	return 0;
}
