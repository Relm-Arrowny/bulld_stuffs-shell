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

static int compare_strings(const void *a, const void *b) {
    const char *str_a = *(const char **)a;
    const char *str_b = *(const char **)b;
    return strcmp(str_a, str_b);
}

static void output_partial_match_command( char *buffer, int *len, const char *match){
    for (int j = *len; j< strlen(match); j++) {
        char temp = match[j];
        buffer[(*len)++] = temp;
        printf("%c", temp);
    }
    fflush(stdout);
}

static void output_match_command( char *buffer, int *len, const char *match){
    output_partial_match_command(buffer, len, match );
    buffer[(*len)++] = ' ';
    printf(" ");
    fflush(stdout);
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
    char *buffer = NULL;
    int len = 0;
    char last_char = '\0';
    int capacity = 4;
    
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
            int matched = 0;
            char **matches = calloc(capacity, sizeof(char *));
            if (matches == NULL) {
                return NULL;
            }


            //builtins
            for (int i = 0; builtins[i] != NULL; i++) {
                if (strncmp(builtins[i], buffer, len) == 0) {
                    matches[matched] = strdup(builtins[i]);
                    matched++;
                    matches[matched] = NULL;
                    if (matched >= capacity) {
                        matches = increase_string_list_capacity(matches, &capacity, matched);
                    }
                }
                
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

                        matches[matched] = strdup(entry->d_name);
                        matched++;
                        matches[matched] = NULL;
                        if (matched >= capacity) {
                            matches = increase_string_list_capacity(matches, &capacity, matched);
                        }
                    }
                }
                closedir(directory);
            }
            free_string_list(path_list);

            qsort(matches, matched, sizeof(char *), compare_strings);
            int unique = 0;
            for (int i = 0; matches[i] != NULL; i++){
                if (i>0 && strcmp(matches[i], matches[unique-1]) == 0){
                    free(matches[i]);
                }
                else{
                    matches[unique++]= matches[i];
                }
            }
            matches[unique] = NULL;
            matched = unique;
            fflush(stdout);
            if (matched==1){
                output_match_command(buffer, &len, matches[0]);
            }
            else if ( matched>1 && last_char != '\t'){
                int common_len = 0;
                char *partial_match;
                partial_match = malloc(4096);
                while (matches[0][common_len] != '\0' &&
                    matches[0][common_len] == matches[matched - 1][common_len]) {
                    partial_match[common_len] = matches[0][common_len];
                    common_len++;
                }
                partial_match[common_len] = '\0';
                output_partial_match_command(buffer, &len, partial_match);
            }
            else if( matched>1 && last_char == '\t') {
                printf("\n");
                for (int i = 0; matches[i] != NULL; i++){
                    printf("%s ", matches[i]);
                }
                printf("\n$ %s",buffer);
                fflush(stdout);
            }
            else{
                printf("\a");
                fflush(stdout);
            }
            free_string_list(matches);
            last_char = c;
            continue;
        }
        if (c != '\r' && c != '\n' ) {
            buffer[len++] = c;
            buffer[len] = '\0';
        }
        printf("%c",c);
        fflush(stdout);
        last_char = c;
    }while (c != '\r' && c != '\n');

    fflush(stdout);
    cooked_mode();
    return buffer;
}