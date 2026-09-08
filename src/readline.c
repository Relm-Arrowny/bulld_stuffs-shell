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

static void output_partial_match_command( char *buffer, int *len, const char *match, int prefix_len){
    for (int j = prefix_len; j< strlen(match); j++) {
        char temp = match[j];
        buffer[(*len)++] = temp;
        printf("%c", temp);
    }
    fflush(stdout);
}

static void output_match_command( char *buffer, int *len, const char *match, int prefix_len){
    output_partial_match_command(buffer, len, match,prefix_len );
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
        //do back space
        if (c == 127 || c == '\b'){
            if (len>0){
                buffer[--len] = '\0';
                printf("\b \b");
                fflush(stdout);
            }
            continue;
        }
        // handle tab
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

            // split the command when there is space
            char *last_space = strrchr(buffer, ' ');
            char *prefix = (last_space != NULL) ? last_space + 1 : buffer;
            int prefix_len = strlen(prefix);

            if (last_space == NULL){
                //builtins
                for (int i = 0; builtins[i] != NULL; i++) {
                    if (strncmp(builtins[i], prefix, prefix_len) == 0) {
                        matches[matched] = strdup(builtins[i]);
                        matched++;
                        matches[matched] = NULL;
                        if (matched >= capacity) {
                            matches = increase_string_list_capacity(matches, &capacity, matched);
                        }
                    }
                    
                }
                //in path
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
            }
            else{
                char *last_slash = strrchr(buffer, '/');
                char *path_prefix = (last_slash != NULL) ? last_slash + 1 : buffer;
                int path_prefix_len = strlen(path_prefix);
                DIR *directory = NULL;

                if (last_slash == NULL){
                    directory = opendir(".");

                }
                else{
                    char dir[PATH_MAX];
                    int dir_len = (int)(last_slash - prefix);
                    char *dir_path = strndup(prefix, dir_len);
                    directory = opendir(dir_path);
                    free(dir_path);
                }    
                if (directory != NULL) {
                    struct dirent *entry;
                    while ((entry = readdir(directory)) != NULL) {
                        if (entry->d_name[0] == '.' && path_prefix[0] != '.') {
                            continue;
                        }
                        if (strncmp(entry->d_name, path_prefix, path_prefix_len) == 0) {
                            matches[matched] = strdup(entry->d_name);
                            // printf("%s", matches[matched]);
                            // fflush(stdout);
                            matched++;
                            matches[matched] = NULL;

                            if (matched >= capacity) {
                                matches = increase_string_list_capacity(matches, &capacity, matched);
                            }
                        }
                    }
                    closedir(directory);
                    prefix_len = path_prefix_len;
                }

            }
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
                output_match_command(buffer, &len, matches[0],prefix_len);
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
                printf("\a");
                fflush(stdout);
                output_partial_match_command(buffer, &len, partial_match,prefix_len);
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