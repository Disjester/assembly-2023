#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "constants.h"
#include "nodes.h"
#include <stdio.h>

bool validateVariableName (char *name);
void preproccessor(char *file_name);

void tokenizeInput(char *input, char **tokens, int *num_tokens);

char *my_strdup(const char *str);

CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int* pnum_tokens);

void freeLinkedList(CodeNode* head);

int getLine(char* line, Error* error, FILE* file);

void cleanLine(char* line);

void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, Error* error, int* pnum_tokens, char** tokens);

void macrosToValues(CodeNode** code, MacroNode** macros, char *tokens[], int* pnum_tokens, Error* error);

void* allocateMemory(size_t size, Error* error);

void handleError(Error* error);
#endif