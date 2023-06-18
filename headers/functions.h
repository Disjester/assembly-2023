#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include "constants.h"


void tokenize_input(char *input, char **tokens, int *num_tokens);
CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int num_tokens);
void freeLinkedList(CodeNode* head);
int getLine(char* line, Error* error, FILE* file);
void clean_line(char* line);
void scanCodeForMacroDefinitions(CodeNode* code_node, MacroNode* macro_node, Error* error);
#endif