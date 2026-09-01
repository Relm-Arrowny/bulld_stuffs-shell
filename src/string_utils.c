
#include "string_utils.h"

#include <stdlib.h>
#include <linux/limits.h>
#include <string.h>

char **split_string(const char * str,const char *delim){
    if (str ==NULL || delim == NULL)
        return NULL;
    size_t capacity = 4;
    size_t count = 0;
 
    char **result = calloc(capacity , sizeof(char*));
    if (result == NULL) {
        return NULL;
    }
    char *str_copy = strdup(str);
    if (str_copy == NULL) {
        free(result);
        return NULL;
    }

    char *saveptr;
    const char *token = strtok_r(str_copy, delim, &saveptr);

    while (token != NULL) {
        if (count+1 >= capacity){
            capacity *= 2;
            char **temp =realloc(result, capacity* sizeof(char*));
            if (temp ==NULL){
                free_string_list(result);
                free(str_copy);
                return temp;
            }
            result = temp;
        }
        result[count]= strdup(token);
        if (result[count] == NULL) {
            free_string_list(result);
            free(str_copy);
            return NULL;
        }
        token = strtok_r(NULL, delim, &saveptr);
        count++;
    }
    result[count] = NULL;
    free(str_copy);
    return result;
}

char **split_string_quotes(const char* input)
{
    int str_len = strlen(input);
    int capacity = 8;
    int token_counter = 0;
    char ** tokens = malloc(sizeof(char *)*capacity);
    char quote_flag = '\0';
    char temp[PATH_MAX];
    int temp_idx = 0;

    for (int i = 0; i<=str_len; i++){
        char c = input[i];
        if (quote_flag == '\0' && c == '\\'){
            if (i+ 1 < str_len) {
                i++;
                temp[temp_idx++] = input[i];
            }
        }
        else if(quote_flag == '"' && c == '\\'){
            char next = input[i + 1];
            if (next == '"' || next == '\\' || next == '$' || next == '\n') {
                i++;
                temp[temp_idx++] = input[i];
            } else {
                temp[temp_idx++] = c;
            }
        }
        
        else if ((c == '\'' || c == '"')){
            if (quote_flag == '\0')
                quote_flag = c;
            else if (quote_flag == c){
                quote_flag = '\0';
            }
            else{
                temp[temp_idx++] = c;
            }
        }
        
        else if (((c == ' ' || c == '\t' || c == '\n' || c == '\r') && quote_flag == '\0') || c == '\0'){
            if (temp_idx > 0) {
                temp[temp_idx] = '\0';

                if (token_counter >= capacity - 1) {
                    capacity *= 2;
                    char **new_tokens = realloc(tokens, sizeof(char *) * capacity);
                    if (!new_tokens) {
                        free_string_list(tokens); 
                        return NULL;
                    }

                    tokens = new_tokens;
                }

                tokens[token_counter++] = strdup(temp);
                temp_idx = 0; 
            }
        }
        else
            temp[temp_idx++] = c;
    }
    tokens[token_counter] = NULL;
    return tokens;
}


void free_string_list(char **list) {
    if (list == NULL) return;
    for (size_t i = 0; list[i] != NULL; i++) {
        free(list[i]);
    }
    free(list);
}