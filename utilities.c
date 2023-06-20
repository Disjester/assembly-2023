
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

void tokenizeInput(char *input, char **tokens, int *num_tokens);

void tokenizeInput(char *input, char **tokens, int *num_tokens) {
    size_t length = strlen(input);
    char *temp = malloc((length + 1) * sizeof(char));
    char *token = strtok(temp, " \t"); /* tokenize input using space and tab characters as delimiters*/
    *num_tokens = 0; /* initialize number of tokens to zero*/
    while (token != NULL && *num_tokens < MAX_TOKENS) { /* iterate over tokens until no more tokens or maximum number of tokens is reached */
        tokens[(*num_tokens)++] = token; /* add token to array and increment number of tokens*/
        token = strtok(NULL, " \t"); /* get next token using the same delimiters*/
    }
    free(temp);
}

/*tokenize_input(updatedInputString, tokens, &num_tokens);*/