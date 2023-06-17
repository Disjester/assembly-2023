#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nodes.h"
#include "constants.h"
#include "errors.h"

int num_tokens;
char *tokens[MAX_TOKENS];

void tokenize_input(char *input, char **tokens, int *num_tokens);