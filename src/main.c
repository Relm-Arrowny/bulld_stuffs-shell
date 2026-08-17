#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  // Flush after every printf
  setbuf(stdout, NULL);
  printf("$ ");

  char userInput[30];
  fgets(userInput, sizeof(userInput), stdin);
  char nothing[1];
  int cmp = strcmp(userInput,nothing);
  if (cmp!=0){
    printf("\n%s", userInput);
  }
  
  return 0;
}
