
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "headers/nodes.h"
#include "headers/constants.h"
#include "headers/errors.h"
#include "headers/functions.h"

int num_tokens;
char *tokens[MAX_TOKENS];

void tokenize_input(char *input, char **tokens, int *num_tokens) {
    char *token = strtok(input, " \t"); /* tokenize input using space and tab characters as delimiters*/
    *num_tokens = 0; /* initialize number of tokens to zero*/
    while (token != NULL && *num_tokens < MAX_TOKENS) { /* iterate over tokens until no more tokens or maximum number of tokens is reached */
        tokens[(*num_tokens)++] = token; /* add token to array and increment number of tokens*/
        token = strtok(NULL, " \t"); /* get next token using the same delimiters*/
    }
}

/*tokenize_input(updatedInputString, tokens, &num_tokens);*/