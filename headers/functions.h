#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "constants.h"

int num_tokens;
char *tokens[MAX_TOKENS];

void tokenize_input(char *input, char **tokens, int *num_tokens);
#endif