
#include "helper.h"
const char *builtins[] = {"exit", "echo", "type"};

int checkType(char* input){
    size_t count = sizeof(builtins) / sizeof(builtins[0]);

    for (size_t i = 0; i < count; i++){
        if (strcmp(input, builtins[i]) == 0) 
            return 1;
    }
    return 0;
}

void noCommand(char* com){
    printf("%s: command not found\n", com);
}
