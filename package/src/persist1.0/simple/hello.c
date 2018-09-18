#include "stplugin.h"

STDLL stata_call(int argc, char *argv[])
{
	static int z = 1;
	char buf[80] ;
	snprintf(buf, 80, "The value of z is %d\n", z) ;
	SF_display(buf);
	z = z + 1;

	return(0) ;
}
