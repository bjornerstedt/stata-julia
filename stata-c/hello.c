
#include "stplugin.h"

STDLL stata_call(int argc, char *argv[])
{
	jl_init();

	char macname[40];
	char buf[80];
	// SF_macro_save("macro", "macname");
	// Get global macro. For local macro, an initial underscore is needed.
	// SF_macro_use("_macro", macname, 39);
	SF_macro_use("macro", macname, 39);
	snprintf(buf, 80, "Macro value: %s\n", macname);
	SF_display(buf);

	snprintf(buf, 80, "Row & col: %d %d\n", SF_row("A"), SF_col("A"));
	SF_display(buf);

	// Copy matrix
	ST_double z, w;
	ST_double k = 2.0;
	ST_int retval = 0;
	ST_int i, j;
	ST_retcode rc ;

	// NOTE that Stata uses 1 based index!
	ST_int rows = SF_row("A");
	ST_int cols = SF_col("A");
	for( i = 1; i <= rows; i++) {
		for( j = 1; j <= cols; j++) {
			if(rc = SF_mat_el("A", i, j, &z)) return(rc) ;
			if(rc = SF_mat_store("B", i, j, 2*z)) return(rc) ;
		}
	}

	// dataset
	if(SF_nvars() < 3) return(22) ;
	for(j = SF_in1(); j <= SF_in2(); j++) {
		if(SF_ifobs(j)) {
			if(rc = SF_vdata(1, j, &z)) return(rc) ;
			if(rc = SF_vdata(2, j, &w)) return(rc) ;
			if(rc = SF_vstore(3, j, z * w)) return(rc) ;
		}
	}

	if (0) {
		SF_error("What an error!\n");
		// Stata error code
		return(99);
	}
	SF_scal_save("x", 1.1);
	return(retval) ;
}
