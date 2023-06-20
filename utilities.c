
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

void tokenizeInput(char *input, char **tokens, int *num_tokens);

void tokenizeInput(char *input, char **tokens, int *num_tokens) {
    size_t length = strlen(input);
    char *temp = malloc((length + 1) * sizeof(char));
    strcpy(temp, input);  /* Copy input string into temp */

    char *token = strtok(temp, " ");
    *num_tokens = 0;
    while (token != NULL && *num_tokens < MAX_TOKENS) {
        tokens[*num_tokens] = strdup(token);  /* Duplicate and store token */
        (*num_tokens)++;
        token = strtok(NULL, " ");
    }

    free(temp);
}
/*tokenize_input(updatedInputString, tokens, &num_tokens);*/