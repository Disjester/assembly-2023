
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

void tokenizeInput(char *input, char **tokens, int *num_tokens);

void tokenizeInput(char *input, char **tokens, int *num_tokens) {
    size_t length = strlen(input);
    char *token;
    char *temp;
    temp = malloc((length + 1) * sizeof(char));
    strcpy(temp, input);  /* Copy input string into temp */

    token = strtok(temp, " ");
    *num_tokens = 0;
    while (token != NULL && *num_tokens < MAX_TOKENS) {
        tokens[*num_tokens] = my_strdup(token);  /* Duplicate and store token */
        (*num_tokens)++;
        token = strtok(NULL, " ");
    }

    free(temp);
}

char *my_strdup(const char *str) {
    size_t length = strlen(str);
    char *duplicate = malloc(length + 1);  /* Allocate memory for the duplicate string*/
    if (duplicate != NULL) {
        strcpy(duplicate, str);  /* Copy the string into the allocated memory*/
    }
    return duplicate;
}