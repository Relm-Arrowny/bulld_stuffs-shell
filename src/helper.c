
#include "helper.h"
char commands[] = "type echo exit";

int checkType(char* input){
    char arg[100];
    strcpy(arg,input);
    arg[strcspn(arg, "\n")] = '\0';
    char *res = strstr(commands, arg);
    if (res != NULL) return 1;
    else return 0;
}

void noCommand(char* com){
    printf("%s: command not found\n", com);
}
