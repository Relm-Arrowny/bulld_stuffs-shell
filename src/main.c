#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]) {
  // Flush after every printf
  //setbuf(stdout, NULL);

  char userInput[30];
  char nothing[1];
  while(1){
    printf("$ ");
    fgets(userInput, sizeof(userInput), stdin);
    userInput[strcspn(userInput, "\n")] = '\0';
    
    int cmp = strcmp(userInput,nothing);
    if (cmp>0){
      printf("%s: command not found\n", userInput);
    }
  }
  return 0;
}
