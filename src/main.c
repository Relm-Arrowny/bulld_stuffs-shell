#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");

  char userInput[30];
  fgets(userInput, sizeof(userInput), stdin);
  char nothing[1];
  int cmp = strcmp(userInput,nothing);
  if (cmp!=0){
    printf("\n%s: command not found", userInput);
  }
  
  return 0;
}
