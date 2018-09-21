#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"

JULIA_DEFINE_FAST_TLS();

// Stata plugin entry point
STDLL stata_call(int argc, char *argv[]) {
	jl_init();
	int retval = main(argc, argv);
	jl_atexit_hook(0);
	return retval;
}

// C application entry point
int main(int argc, char *argv[]) {
	int retval = 0;
	char buf[80] ;

	if (argc < 2) {
		SF_error("Internal error. The ADO file has sent the wrong number of pars");
		return 1;
	}
	char* function = argv[0];
	char* using = (argc == 2)?argv[1]:"";

	jl_value_t *stata;
	jl_value_t *stata_data;
	jl_eval_string("using StataJulia");
	if(jl_exception_occurred()) {
		snprintf(buf, 80, "using StataJulia error %s\n", jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 1019;
	}
	if (strlen(using)) {
		snprintf(buf, 80, "using %s", using) ;
		jl_eval_string(buf);
		if (jl_exception_occurred()) {
			snprintf(buf, 80, "using %s failed, error: %s.\n", using, jl_typeof_str(jl_exception_occurred())) ;
			SF_error(buf);
			return 101;
		}
	}
	stata = jl_eval_string("StataJulia.getInstance()");
	if(stata == NULL || jl_exception_occurred()) {
		char buf[80] ;
		snprintf(buf, 80, "StataJulia.getInstance() failed:  %s\n" ,jl_typeof_str(jl_exception_occurred()));
		SF_error(buf);
	}
	// TODO: Fix checking
	// snprintf(buf, 80, "StataJulia.isvalidfunction(%s.%s)", using, function );
	// SF_error(buf );
	// if (!jl_unbox_int32(jl_eval_string(buf)) ) {
	// 	snprintf(buf, 80, "Could not find function %s in module %s\n" , function, using);
	// 	SF_error(buf);
	// 	return 999;
	// }

	// function without parameters is to set data
	stata_data = call_julia(using, function , NULL, NULL, NULL);
	if (stata_data == NULL || jl_exception_occurred()) {
		snprintf(buf, 80, "Calling function %s in %s failed: %s\n", function, using, jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 1019;
	}
	JL_GC_PUSH2(&stata_data, &stata);

	retval = process(using, function, stata, stata_data);

	JL_GC_POP();
	return 0;

}

int process(char *using, char *function, jl_value_t *stata,  jl_value_t *stata_data) {

	int rc = 0;
	if( (rc = matrices(stata, stata_data, 0)) )  return rc ;
	if( (rc = scalars(stata, stata_data, 0)) )  return rc ;
	if( (rc = macros(stata, stata_data, 0)) )  return rc ;

	call_julia(using, function, stata, NULL, NULL);
	if(jl_exception_occurred()) {
		char buf[80] ;
		snprintf(buf, 80, "process(): Could not run Julia function: %s, %s\n", function ,jl_typeof_str(jl_exception_occurred()));
		SF_error(buf);
		return 2341;
	}
	if( (rc = matrices(stata, stata_data, 1)) )  return rc ;
	if( (rc = scalars(stata, stata_data, 1)) )  return rc ;
	if( (rc = macros(stata, stata_data, 1)) )  return rc ;

}
