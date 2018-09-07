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
	if (argc != 4) {
		SF_error("Internal error. The ADO file has sent the wrong number of pars");
		return 1;
	}
	char* function = argv[0];
	char* using = argv[1];
	char* command = argv[2];
	char* varlist = argv[3];

	if (strlen(using) == 0) {
		return execute_command(command);
	}
	if (strlen(function) == 0) {
		SF_error("Either function or command has to be specified.\n");
		return 1;
	}

	int retval = 0;
	char buf[80] ;
	snprintf(buf, 80, "include(\"%s\")", using) ;
	jl_eval_string(buf);
	if (jl_exception_occurred()) {
		snprintf(buf, 80, "File init.jl not executed, error: %s.\n", jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 101;
	}

	// Invoke command
	retval = process( function, varlist);

	jl_atexit_hook(0);
	return(retval) ;
}

// Execute single Julia command, returning the result in a Stata macro
int execute_command(char *command) {
	char buf[80] ;
	if (strlen(command) == 0) {
		SF_error("Either using och command options have to be set");
		return 1;
	}

	// Evaluate command:
	jl_value_t *ret = jl_eval_string(command);
	if (jl_exception_occurred()) {
		SF_error("Could not understand Julia command");
		return 1;
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
