#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "errors.h"
#include "constants.h"
#include "nodes.h"
#include <stdio.h>

/**
 * @brief checks if the string is a label
 * 
 * @param word 
 * @return true 
 * @return false 
 */
bool isLabel(char* word);

void cleanMemory(short* memory);

void firstIteration(short* memory, CodeNode* code, LabelNode* labels, Error* error);

void pushToMemory(int* memory_counter, short* memory, short memoryField);

bool validateVariableName (char *name);

CodeNode* preproccessor(char *file_name);

/**
 * @brief takes a string , and splits it into words base on whiteSpaces.
 *        Note: **tokens need to have allocated memmory - the function doesn't allocate it 
 * 
 * @param input the string you want to tokenize
 * @param tokens the array of strings in which you would save your tokens - need to have memory already allocated
 * @param num_tokens a pointer to where you save the number of tokens 
 */
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


bool checkData(char* line, Error* error);

/**
 * @brief checks if a line is a correct data line or not
 * 
 * @param tokens 
 * @param num_tokens 
 * @param label - a boolean that says if the 1st token is a label or not
 * @return true - if it is a data line and its correct
 * @return false - when its not a data line or an incorrect one 
 */
bool checkDataLine(char** tokens, int num_tokens, bool label);

bool isNumber(char* word);

bool isString(char* string);

/**
 * @brief checks if the word is .data or .string , returns false if neither
 * 
 * @param word 
 * @return 2 - if its a .string 1 if it is a .data 0 if it neither of them 
 */
short isDotType(char* word);

LabelType getLabelType(char* label, LabelNode* LabelNode);

#endif