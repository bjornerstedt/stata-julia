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
		return SJ_execute_command(command);
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
	retval = SJ_process( function, varlist);

	jl_atexit_hook(0);
	return(retval) ;
}
