#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"



// ****************************************************


int matrices(jl_value_t *stata, jl_value_t *stata_data, int update) {
	int i = 1;
	int rv;
	char* name = getNameFromList(stata_data, "matrices", update, i++);
    while( strlen(name) ) {
		if((rv = matrix(stata, name, update)) )
			return rv;
		name = getNameFromList(stata_data, "matrices", update, i++);
    }
    return 0;
}

// ---------------- Get individual values --------------------

// Get Stata global array by name
int matrix(jl_value_t *stata, char* name, int update) {
	char errbuf[80] ;
	ST_int i, j;
	ST_retcode rc ;
	ST_int rows, cols;
	rows = SF_row(name);
	cols = SF_col(name);

	if (rows == 0) {
	  	snprintf(errbuf, 80, "Could not get matrix:%s.\n", name) ;
	  	SF_display(errbuf);
		return 161;
	}

	if (update) {
		ST_double z;
		jl_value_t *x = call_julia("StataJulia", "getMatrix", stata, jl_cstr_to_string(name), NULL);
		if( x == NULL || jl_exception_occurred()) {
			snprintf(errbuf, 80, "Could not get array from Julia: %s\n", name);
			SF_error(errbuf);
			return 177;
		}
		// Get number of dimensions
		int ndims = jl_array_ndims(x);
		size_t rowsj = jl_array_dim(x, 0);
		size_t colsj = jl_array_dim(x, 1);
		if (rowsj != rows || colsj != cols) {
			snprintf(errbuf, 80, "ERROR: matrix dimensions for do not match for: %s", name);
			return 929;
		}

		// Get array pointer
	    double *xData = (double*)jl_array_data(x);
		for( i = 0; i < rows; i++) {
			for( j = 0; j < cols; j++) {
				// NOTE that Stata uses 1 based index!
				z = xData[j + cols * i];
				if((rc = SF_mat_store(name, i + 1, j + 1, z))) return(rc) ;
			}
		}
	} else {
		jl_array_t *x;
		if( (x = create_2D(rows, cols)) == NULL ) return 4343;
		// Get array pointer
	    double *xData = (double*)jl_array_data(x);
		ST_double z;
		for( i = 0; i < rows; i++) {
			for( j = 0; j < cols; j++) {
				// NOTE that Stata uses 1 based index!
					if(SF_mat_el(name, i + 1, j + 1, &z)) return(12312) ;
					xData[j + cols*i] = z;
			}
		}
		if( call_julia("StataJulia", "addMatrix", stata, jl_cstr_to_string(name), (jl_value_t *)x ) == NULL ) {
			SF_error("Could not add array to Julia\n");
			return 3214;
		}

}

	return 0;
}
