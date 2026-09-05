#include "redirection.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>



static int is_redirect(const char *tok) {
    return strcmp(tok, ">") == 0  || strcmp(tok, "1>") == 0  || 
           strcmp(tok, ">>") == 0 || strcmp(tok, "1>>") == 0 || 
           strcmp(tok, "2>") == 0 || strcmp(tok, "2>>") == 0;
}

static int is_append(const char *tok) {
    return strcmp(tok, ">>") == 0 || strcmp(tok, "1>>") == 0 || strcmp(tok, "2>>") == 0;
}

static int is_stderr(const char *tok) {
    return strcmp(tok, "2>") == 0 || strcmp(tok, "2>>") == 0;
}
void setup_redirection(char **com)
{
    for (char **ipter = com; *ipter !=NULL;){
        if (com == NULL) {
            return;
        }
       
        if (is_redirect(*ipter)){
            const char *filename = *(ipter+1);
            if (filename ==NULL){
                fprintf(stderr,"syntax error near unexpected filename 'newline'\n");
                exit(1);
            }
            else{

                int flags = O_WRONLY | O_CREAT | (is_append(*ipter) ? O_APPEND : O_TRUNC);
                int fd = open(*(ipter + 1), flags, 0644);
                if (fd < 0) {
                    perror("open");
                    return;
                }
                int target_fd = is_stderr(*ipter) ? STDERR_FILENO : STDOUT_FILENO;

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

int builtin_redirection_wrapper(char **com, int (*func)(const char **)){
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
