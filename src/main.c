#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>

#include "commands.h"
#include "redirection.h"
#include "string_utils.h"
#include "redirection.h"
#include "readline.h"
int main(int argc, char *argv[]) {
  while(1){
    char *userInput = readline("$ ");
    if (userInput == NULL) {
      break;
    }
    userInput[strcspn(userInput, "\n")] = '\0';
    char ** input_list;
    input_list = split_string_quotes(userInput);

    if (input_list == NULL || input_list[0] == NULL) {
      free_string_list(input_list);
      free(userInput);
      continue;
    }

    const BuiltinCommand *cmd = find_command(input_list[0]);
    if (cmd != NULL){
      builtin_redirection_wrapper(input_list, cmd->func);
    }
    else 
      check_and_run(input_list);
    free_string_list(input_list);
    free(userInput);
  }
  return 0;
}

