// Test C mehtods here
#include <stdio.h>
#include <strings.h>
int getIndices(char* list, char* list2, int tokenIndex[]);

int main(int argc, char *argv[]) {

    char list[] = " n v nv ";
    char list2[] = "v nv";
    int tokenIndex[80];
     getIndices(list, list2, tokenIndex);

     for (size_t i = 0; i < 5; i++) {
            printf("token = %d\n", tokenIndex[i] );
     }

return 2;

}

// Get token positions of tokens in list2 in list and return in tokenIndex
int getIndices(char* list, char* list2, int tokenIndex[]) {
    char* token[80];
    int i = 0;
    char *name = strtok(list, " ");
    while( name != NULL ) {
        char command[80];
        token[i++] = name;
        name = strtok(NULL, " ");
    }
    int token1count = i;

    char* token2[80];
    i = 0;
    name = strtok(list2, " ");
    while( name != NULL ) {
        char command[80];
        token2[i++] = name;
        name = strtok(NULL, " ");
    }
    int token2count = i;

    name = strtok(list, " ");
    for (i = 0; i < token2count; i++) {
        for (size_t j = 0; j < token1count; j++) {
            if (!strcmp(token2[i], token[j]) ) {
                tokenIndex[i] = j + 1; // Stata is 1 based
            }
        }
    }
    return 0;
}
