
#include "stplugin.h"

STDLL stata_call(int argc, char *argv[])
{
	SF_display("Hello World yeah!\n") ;
  SF_scal_save("x", 1.1);
	return(0) ;
}
