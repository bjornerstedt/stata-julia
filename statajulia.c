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
	if (argc != 3) {
		SF_error("Internal error. The ADO file has sent the wrong number of pars");
		return 1;
	}
	char* method = argv[0];
	char* using = argv[1];
	char* command = argv[2];
	char buf[80] ;
	method = "testf!"; /////////// To simplify testing

	if (strlen(using) == 0) {
		return SJ_execute_command(command);
	}
	if (strlen(method) == 0) {
		SF_error("Either method or command has to be specified.\n");
		return 1;
	}

	int retval = 0;
	snprintf(buf, 80, "include(\"%s\")", using) ;
	jl_eval_string(buf);
	if (jl_exception_occurred()) {
		char errbuf[80];
	  snprintf(errbuf, 80, "File init.jl not executed, error: %s.\n", jl_typeof_str(jl_exception_occurred())) ;
	  SF_error(errbuf);
		return 101;
	}

	// Get matrix names from init.jl
	jl_value_t *ret = jl_eval_string("inmatrix");
	if (jl_exception_occurred()) {
		SF_error("inmatrix not found");
		 return 101;
	}
	char *mat1 = jl_string_ptr(ret);
	ret = jl_eval_string("outmatrix");
	if (jl_exception_occurred()) {
		SF_error("outmatrix not found");
		 return 101;
	}
	char *mat2 = jl_string_ptr(ret);

	// Invoke command
	retval = SJ_process( mat1, mat2, method);

	jl_atexit_hook(0);
	return(retval) ;
}
