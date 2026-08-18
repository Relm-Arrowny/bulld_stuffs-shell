
#include "helper.h"

int checkType(char* input){
    if (strncmp(input,"exit",5 )== 0) return 1;
    else if (strncmp(input,"echo",5 )== 0) return 1;
    else if (strncmp(input,"type",5 )== 0) return 1;
    else return 0;
}

void noCommand(char* com){
    printf("%s: command not found\n", com);
}
