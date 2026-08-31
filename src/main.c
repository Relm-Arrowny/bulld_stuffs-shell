#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>
#include "commands.h"
#include "helper.h"
int main(int argc, char *argv[]) {
  // Flush after every printf
  //setbuf(stdout, NULL);


  char userInput[1024];
  while(1){
    printf("$ ");
    fgets(userInput, sizeof(userInput), stdin);
    userInput[strcspn(userInput, "\n")] = '\0';
    char ** input_list;
    input_list = split_string_quotes(userInput);

    if (input_list == NULL || input_list[0] == NULL) {
        free_string_list(input_list);
        continue;
    }
    if (strcmp(input_list[0],"exit")==0){
      free_string_list(input_list);
      break;
    }
    else if (strcmp(input_list[0], "echo") == 0){
      buldtin_redirection_wraper(input_list,custom_echo);
    }
    else if (strcmp(input_list[0],"type")== 0){
      buldtin_redirection_wraper(input_list,checkType);
      }
    else if (strcmp(input_list[0],"pwd" )== 0){
      char cwd[PATH_MAX];
      if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
      } 
      else {
        perror("getcwd() error");
      }
    }
    else if (strcmp(input_list[0], "cd")== 0){
      changeDir(input_list[1]);
    }
    else 
      checkAndRun(input_list);
    free_string_list(input_list);
  }
  return 0;
}

