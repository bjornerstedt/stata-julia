#include "stplugin.h"
#include <julia.h>
#include <strings.h>
#include "statajulia.h"




// ---------------- Get individual values --------------------

// Get Stata global array by name
jl_array_t *get_matrix(char* name) {
	ST_int i, j;
	ST_retcode rc ;
	ST_int rows, cols;
	rows = SF_row(name);
	cols = SF_col(name);

	if (rows == 0) {
		char errbuf[80] ;
	  	snprintf(errbuf, 80, "Could not get matrix:%s.\n", name) ;
	  	SF_display(errbuf);
		return NULL;
	}

	jl_array_t *x;
	if( (x = create_2D(rows, cols)) == NULL ) return NULL;

	// Get array pointer
    double *xData = (double*)jl_array_data(x);
	ST_double z;
	for( i = 0; i < rows; i++) {
		for( j = 0; j < cols; j++) {
			// NOTE that Stata uses 1 based index!
				if(SF_mat_el(name, i + 1, j + 1, &z)) return(NULL) ;
				xData[j + cols*i] = z;
		}
	}
	return x;
}

// Set either by name (matrices) or by index (variables)
int set_matrix(char* name, jl_array_t *x) {
	char errbuf[80] ;
	ST_int rows, cols;
	rows = SF_row(name);
	cols = SF_col(name);

	ST_int i, j;
	ST_retcode rc ;
	ST_double z;

	if (rows == 0) {
	    snprintf(errbuf, 80, "Could not get matrix: %s\n", name) ;
	    SF_display(errbuf);
		return 989;
	}

	// Get number of dimensions
	int ndims = jl_array_ndims(x);
	size_t rowsj = jl_array_dim(x, 0);
	size_t colsj = jl_array_dim(x, 1);
	if (rowsj != rows || colsj != cols) {
		snprintf(errbuf, 80, "ERROR: matrix dimensions for do not match for: %s", name);
		SF_display(errbuf);
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
	return 0;
}
