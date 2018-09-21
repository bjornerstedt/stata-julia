#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include <stdio.h>
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
	char* module = (argc >= 1)?argv[1]:"";
	char* command = (2 < argc)?argv[2]:"";
	char* save = (3 < argc)?argv[3]:"";

	jl_value_t *stata;
	jl_value_t *stata_data;
	jl_eval_string("using StataJulia");
	if(jl_exception_occurred()) {
		snprintf(buf, 80, "Could not find the StataJulia package,  %s\n", jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 1019;
	}
	if (strlen(module)) {
		snprintf(buf, 80, "using %s", module) ;
		jl_eval_string(buf);
		if (jl_exception_occurred()) {
			snprintf(buf, 80, "using %s failed, error: %s.\n", module, jl_typeof_str(jl_exception_occurred())) ;
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
	// snprintf(buf, 80, "StataJulia.isvalidfunction(%s.%s)", module, function );
	// SF_error(buf );
	// if (!jl_unbox_int32(jl_eval_string(buf)) ) {
	// 	snprintf(buf, 80, "Could not find function %s in module %s\n" , function, module);
	// 	SF_error(buf);
	// 	return 999;
	// }

	// function without parameters is to set data
	stata_data = call_julia(module, function , NULL, NULL, NULL);
	if (stata_data == NULL || jl_exception_occurred()) {
		snprintf(buf, 80, "Calling function %s in %s failed: %s\n", function, module, jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 1019;
	}
	JL_GC_PUSH2(&stata_data, &stata);

	static char* listnames[10];
	int i = 0;
	listnames[i++] = "savefile";
	listnames[i++] = "get_variables";
	listnames[i++] = "set_variables";
	listnames[i++] = "get_matrices";
	listnames[i++] = "set_matrices";
	listnames[i++] = "get_scalars";
	listnames[i++] = "set_scalars";
	listnames[i++] = "get_macros";
	listnames[i++] = "set_macros";
	for ( i = 3; i < argc; i++) {
		if (strlen(argv[i])) {
			julia_set_varlist(stata_data, listnames[i - 3], argv[i]);
		}
	}

	if ( strlen(save) ) {
		snprintf(buf, 80, "Saving data to file: %s\n", save) ;
		SF_display(buf);
		function = "serializeData";
	}
	if (strlen(command)) {
		return execute_command(command);
	}
	if (function != NULL && strlen(function) == 0) {
		SF_error("Either function or command has to be specified.\n");
		return 211;
	}

	retval = process(module, function, stata, stata_data);
	JL_GC_POP();
	return retval;
}

int process(char *module, char *function, jl_value_t *stata,  jl_value_t *stata_data) {

	int rc = 0;
	if( (rc = variables(stata, stata_data, 0)) )  return rc ;
	if( (rc = matrices(stata, stata_data, 0)) )  return rc ;
	if( (rc = scalars(stata, stata_data, 0)) )  return rc ;
	if( (rc = macros(stata, stata_data, 0)) )  return rc ;

	call_julia(module, function, stata, NULL, NULL);
	if(jl_exception_occurred()) {
		char buf[80] ;
		snprintf(buf, 80, "process(): Could not run Julia function: %s, %s\n", function ,jl_typeof_str(jl_exception_occurred()));
		SF_error(buf);
		return 2341;
	}
	if( (rc = variables(stata, stata_data, 1)) )  return rc ;
	if( (rc = matrices(stata, stata_data, 1)) )  return rc ;
	if( (rc = scalars(stata, stata_data, 1)) )  return rc ;
	if( (rc = macros(stata, stata_data, 1)) )  return rc ;
	return 0;
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
