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

	if (argc < 1) {
		SF_error("Internal error. The ADO file has sent the wrong number of pars");
		return 1;
	}
	char* function = argv[0];
	char* using = (argc ==2)?argv[1]:"";

	jl_value_t *stata;
	jl_value_t *stata_data;

	// jl_eval_string("include(\"StataJulia.jl\")");
	jl_eval_string("using StataJulia");
	if(jl_exception_occurred()) {
		snprintf(buf, 80, "using StataJulia error %s\n", jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 1019;
	}
	if (strlen(using)) {
		snprintf(buf, 80, "include(\"%s.jl\")", using) ;
		jl_eval_string(buf);
		if (jl_exception_occurred()) {
			snprintf(buf, 80, "File %s not executed, error: %s.\n", using, jl_typeof_str(jl_exception_occurred())) ;
			SF_error(buf);
			return 101;
		}
		snprintf(buf, 80, "using %s", using) ;
		jl_eval_string(buf);
	}
	stata = jl_eval_string("StataJulia.getInstance()");
	if(stata == NULL || jl_exception_occurred()) {
		char buf[80] ;
		snprintf(buf, 80, "getInstance(): Could not init:  %s\n" ,jl_typeof_str(jl_exception_occurred()));
		SF_error(buf);
	}
	// function without parameters is to set data

	stata_data = jl_eval_string("StataJulia.getInit()" );
	// stata_data = call_julia("StataJulia", "getInit" , NULL, NULL, NULL);
	// stata_data = call_julia("StataJulia", function , NULL, NULL, NULL);
	if (stata_data == NULL || jl_exception_occurred()) {
		snprintf(buf, 80, "initParams error:%s. %s\n", using, jl_typeof_str(jl_exception_occurred())) ;
		SF_error(buf);
		return 1019;
	}
	JL_GC_PUSH2(&stata_data, &stata);

	int rc = 0;
	if( (rc = matrices(stata, stata_data)) )  return rc ;

	call_julia("StataJulia", function, stata, NULL, NULL);
	if(jl_exception_occurred()) {
		char buf[80] ;
		snprintf(buf, 80, "process(): Could not run Julia function: %s, %s\n", function ,jl_typeof_str(jl_exception_occurred()));
		SF_error(buf);
		return 2341;
	}

	if( (rc = set_matrices(stata, stata_data)) )  return rc ;

	JL_GC_POP();
	return 0;
}

// ****************************************************

char* getNameFromList(jl_value_t *stata_data, char* namelist, int index) {
	jl_function_t *func = jl_get_function(jl_eval_string("StataJulia"), "nameGetVar");
	if (func == NULL || jl_exception_occurred()) {
		SF_display("Could not find function nameGetVar.");
		return "";
	}
	jl_value_t *ret = jl_call3(func, stata_data, jl_cstr_to_string(namelist), jl_box_int32(index));
	if(ret == NULL || jl_exception_occurred()) {
		char buf[80] ;
		snprintf(buf, 80, "getNameFromList(): Could not get index: %s\n" ,jl_typeof_str(jl_exception_occurred()));
		SF_error(buf);
		return "";
	}
	return (char *)jl_string_ptr(ret);
}


int matrices(jl_value_t *stata, jl_value_t *stata_data) {
	int i = 1;
	char* name = getNameFromList(stata_data, "get_matrices", i++);
    while( strlen(name) ) {
		jl_array_t *x;
		if((x = get_matrix(name)) == NULL)
			return 10111;
		if( call_julia("StataJulia", "addMatrix", stata, jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) {
			SF_error("Could not add array to Julia\n");
			return 3214;
		}
		printf("Matrix %s\n", name);
		// GC: Associate matrix to collection:
		jl_gc_wb(stata, x);
		// Get next name
		name = getNameFromList(stata_data, "get_matrices", i++);
    }
    return 0;
}

int set_matrices(jl_value_t *stata, jl_value_t *stata_data) {
	char buf[200];
	int i = 1;
	char* name = getNameFromList(stata_data, "set_matrices", i++);
	while( strlen(name) ) {
		char command[80];
		jl_value_t *x = call_julia("StataJulia", "getMatrix", stata, jl_cstr_to_string(name), NULL);
		if( x == NULL || jl_exception_occurred()) {
			snprintf(buf, 80, "Could not get array from Julia: %s\n", name);
			SF_error(buf);
	  		return 177;
	  	}

		int rc = 0;
		if ((rc = set_matrix(name, (jl_array_t *)x) ) ){
			snprintf(buf, 80, "Setting Stata matrix failed: %s\n", name);
			SF_error(buf);
			return rc;
		}
		// Get next name
		name = getNameFromList(stata_data, "set_matrices", i++);
    }
    return 0;
}