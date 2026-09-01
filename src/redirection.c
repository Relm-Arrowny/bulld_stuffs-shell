#include "redirection.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

void setup_redirection(char **com)
{
    for (char **ipter = com; *ipter !=NULL;){
        if (com == NULL) {
            return;
        }
        if ((strcmp(*ipter,">") == 0 || strcmp(*ipter,"1>")== 0|| strcmp(*ipter,"2>")== 0) ){
            char *filename = *(ipter+1);
            if (filename ==NULL){
                fprintf(stderr,"syntax error near unexpected filename 'newline'\n");
                exit(1);
            }
            else{
                int fd = open(*(ipter+1),O_WRONLY | O_CREAT | O_TRUNC, 0644 );
                if (fd < 0) {
                    perror("open");
                    return;
                }
                int target_fd = (strcmp(*ipter, "2>") == 0) ? STDERR_FILENO : STDOUT_FILENO;

                if (dup2(fd, target_fd) < 0) {
                    perror("dup2");
                    close(fd);
                    return;
                }
                close(fd);

                free(*ipter);
                free(*(ipter+1));

                char **curr = ipter;
                while (*(curr + 2) != NULL) {
                    *curr = *(curr + 2);
                    curr++;
                }
                *curr = NULL;

                continue;
            }
        }
        ipter++;

    }
}

int builtin_redirection_wraper(char **com, int (*func)(const char **)){
    int saved_stdout = dup(STDOUT_FILENO); 
    if (saved_stdout < 0) {
        perror("dup failed");
        return -1;
    }
    setup_redirection(com);
    int success = func((const char **) com);
    fflush(stdout);
    dup2(saved_stdout, STDOUT_FILENO);
    close(saved_stdout);
    return success;
}
