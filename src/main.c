#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helper.h"
#include <linux/limits.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  //setbuf(stdout, NULL);


  char userInput[1024];
  while(1){
    printf("$ ");
    fgets(userInput, sizeof(userInput), stdin);
    userInput[strcspn(userInput, "\n")] = '\0';
    
    if (strcmp(userInput,"exit")==0)
      break;
    else if (strncmp(userInput,"echo ",5 )== 0)
      printf("%s\n", userInput + 5);
    else if (strncmp(userInput,"type ",5 )== 0){
      checkType(userInput + 5); 
      }
    else if (strncmp(userInput,"pwd",3 )== 0){
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
      } 
      else {
        perror("getcwd() error");
      }
    }
    else if (strncmp(userInput,"cd ",3 )== 0){
      chdir(userInput+3);
    }
    else 
      checkAndRun(userInput);
  }
  return 0;
}

