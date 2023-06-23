#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "constants.h"
#include "nodes.h"
#include <stdio.h>

/**
 * @brief 
 * 
 * @param input 
 * @param tokens 
 * @param num_tokens 
 */
void tokenizeInput(char *input, char **tokens, int *num_tokens);

char *my_strdup(const char *str);

/**
 * @brief Create a Linked List From File object
 * 
 * @param file the file to be opened and read
 * @param error 
 * @param tokens pointer to an array of tokens, that will be used in the future with the tokenizeInput function()
 * @param pnum_tokens pointer to the number of num_tokens
 * @return CodeNode* 
 */
CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int* pnum_tokens);

/**
 * @brief 
 * 
 * @param head 
 */
void freeLinkedList(CodeNode* head);

/**
 * @brief Get the Line object
 * 
 * @param line 
 * @param error 
 * @param file 
 * @return int 
 */
int getLine(char* line, Error* error, FILE* file);

/**
 * @brief 
 * 
 * @param line 
 */
void cleanLine(char* line);

/**
 * @brief 
 * 
 * @param code_node 
 * @param macro_node 
 * @param error 
 */
void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, Error* error, int* pnum_tokens, char** tokens);


void macrosToValues(CodeNode** code, MacroNode** macros, char *tokens[], int* pnum_tokens);
#endif