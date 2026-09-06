#include "readline.h"

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "string_utils.h"
#include <commands.h>

static struct termios orig_termios;
static int atexit_registered = 0;
static int is_raw_mode = 0;


static void match_commands( char *buffer, int *len, const char *match){
     if (strncmp(match, buffer, *len) == 0) {
        for (int j = *len; j< strlen(match); j++) {
            char temp = match[j];
            buffer[(*len)++] = temp;
            printf("%c", temp);
        }
        buffer[(*len)++] = ' ';
        printf(" ");
        fflush(stdout);
    }
}
void cooked_mode() {
    if (!is_raw_mode) {
        return;
    }
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
    is_raw_mode = 0;
}

void raw_mode(){
    if (is_raw_mode){
        return;

    }
    struct termios  raw;
    if (!atexit_registered) {
        atexit(cooked_mode);
        atexit_registered = 1;
    }
    tcgetattr(STDIN_FILENO, &orig_termios);
    raw = orig_termios;

    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN]  = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    is_raw_mode = 1;
}

char *readline(const char *prompt) {
    raw_mode();
    int matched = 0;
    char *buffer = NULL;
    int len = 0;
    buffer = malloc(4096);
    if (buffer == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    printf("%s", prompt);
    fflush(stdout);
    char c;
    do {
        ssize_t bytes_read = read(STDIN_FILENO, &c, 1);
        if (bytes_read < 0) {
            perror("read");
            free(buffer);
            exit(EXIT_FAILURE);
        }
        else if (bytes_read == 0) {
            cooked_mode();
            free(buffer);
            return NULL;
        }
        if (c == 127 || c == '\b'){
            if (len>0){
                buffer[--len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
            continue;
        }
        if (c == '\t') {
            if (len == 0){
                printf("\a");
                fflush(stdout);
                continue;
            }
            //builtins
            for (int i = 0; builtins[i] != NULL; i++) {
                if (strncmp(builtins[i], buffer, len) == 0) {
                    match_commands(buffer, &len, builtins[i]);
                    matched = 1;
                    break;
                }
                
            }
            if (matched>0) {
                continue;
            }
            char *path_env = getenv("PATH");
            if (path_env == NULL || *path_env == '\0') {
                continue;
            }       
            char ** path_list = split_string(path_env, ":");      
            for (int i = 0; path_list[i] !=NULL;i++){
                DIR* directory = opendir(path_list[i]);
                if (directory == NULL) {
                    continue;
                }                   
                struct dirent* entry = NULL;
                while ((entry = readdir(directory)) != NULL) {
                    if (strncmp(entry->d_name, buffer, len) == 0) {
                        match_commands(buffer, &len, entry->d_name);
                        matched = 1;
                        break;
                    }
                }
                closedir(directory);
            }      
            free_string_list(path_list);
            if (!matched) {
                printf("\a");
                fflush(stdout);
            }
            continue;
        }
        if (c != '\r' && c != '\n') {
            buffer[len++] = c;
            buffer[len] = '\0';
        }
        printf("%c",c);
        fflush(stdout);
    }while (c != '\r' && c != '\n');
    fflush(stdout);
    cooked_mode();
    return buffer;
}