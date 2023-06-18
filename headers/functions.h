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
void tokenize_input(char *input, char **tokens, int *num_tokens);

/**
 * @brief Create a Linked List From File object
 * 
 * @param file 
 * @param error 
 * @param tokens 
 * @param num_tokens 
 * @return CodeNode* 
 */
CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int* num_tokens);

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
void clean_line(char* line);

/**
 * @brief 
 * 
 * @param code_node 
 * @param macro_node 
 * @param error 
 */
void scanCodeForMacroDefinitions(CodeNode* code_node, MacroNode* macro_node, Error* error);
#endif