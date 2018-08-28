#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char * argv[]) {
    //  char *x = getenv("JULIA_HOME");
    // printf(x);
    printf("JULIA_HOME : %s\n", getenv("JULIA_HOME"));
    printf("\n");
    return 0;
}
