#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"

int main(int argc, char *argv[]) {
  // Flush after every printf
  //setbuf(stdout, NULL);


  char userInput[1024];
  while(1){
    //printf("$ ");
    fgets(userInput, sizeof(userInput), stdin);
    userInput[strcspn(userInput, "\n")] = '\0';
    
    if (strcmp(userInput,"exit")==0)
      break;
    else if (strncmp(userInput,"echo ",5 )== 0)
      printf("%s\n", userInput + 5);
    else if (strncmp(userInput,"type ",5 )== 0){
      char * com = userInput + 5;
        if (checkType(com)) 
          printf("%s is a shell builtin\n", com);
        else if(checkTypeDefaultPath(com));
        else
          printf("%s: not found\n", com);
      }
    else
      noCommand (userInput);
  }

  return 0;
}

