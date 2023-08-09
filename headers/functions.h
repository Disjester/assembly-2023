#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "constants.h"
#include "structures.h"
#include <stdio.h>


/**
 * @brief takes a line of code with command, and checks if its legal
 * 
 * @param tokens 
 * @param num_tokens 
 * @param label - Is the line starts with a label ? 
 * @return int  - > returns L = the number of memory words in the command line , -1 otherwise
 */
int checkCommandLine(char** tokens, int num_tokens, bool label, LabelNode* LabelPtr, Error* error, bool is_first_iteration, bool* stop_flag);

/**
 * @brief returns opcode of a command if it is, otherwise returns -1
 * 
 * @param word 
 * @return short 
 */
short checkCommand(char* word);

OperandType checkOperand(char* operand, LabelNode* LabelPtr, Error* error, bool is_first_iteration);
/**
 * @brief checks if the string is a label
 * 
 * @param word 
 * @param colon - if the end of label has to have ":" - put true, otherwise false
 * @return true 
 * @return false 
 */
bool isLabel(char* word, bool colon);

void moveDataToMemory(short* data_memory, int* data_memory_idx, short* memory, int* memory_idx, Error* error);

void createOutputFiles (char* file_name, LabelNode* labels, short* memory, int* memory_idx, int IC, int DC, LabelNode* externals, bool* is_print, Error* error);

void createBinaryWordByType(LabelNode* labels, OperandType op_type, char* operand, short* memory, int* memory_idx, bool is_first_iteration, Error* error);

void createFileWithLabelType(char* file_name, LabelNode* labels, LabelType label_type, Error* error);

void cleanMemory(short* memory);

void updateEntryLabels(LabelNode* labels, char** tokens, int num_tokens, int token_idx);

char* removeColon(char* str);

void insertNewLabel(LabelNode** label, char* label_name, LabelType label_type, int* memory_idx, bool* is_print, Error* error);

void firstIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode** labels, int* DC, int* IC, bool* is_print, Error* error);

void secondIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode* labels, int* DC, int* IC, Error* error, char* file_name, LabelNode* externals, bool* is_print);

void pushToMemory(int* memory_counter, short* memory, short memoryField, Error* error);

int getOperandsNumberByOpcode(short opcode);

bool validateVariableName (char *name);

CodeNode* preproccessor(char *file_name, bool* is_print, Error* error);

/**
 * @brief takes a string , and splits it into words base on whiteSpaces.
 *        Note: **tokens need to have allocated memmory - the function doesn't allocate it 
 * 
 * @param input the string you want to tokenize
 * @param tokens the array of strings in which you would save your tokens - need to have memory already allocated
 * @param num_tokens a pointer to where you save the number of tokens 
 */
void tokenizeInput(char *input, char **tokens, int *num_tokens, bool* is_print, Error* error);

char *my_strdup(const char *str, bool* is_print, Error* error);

CodeNode* createLinkedListFromFile(FILE* file, char *tokens[], int* pnum_tokens, bool* is_print, Error* error);

short createCommandBinaryWord(char** tokens, int num_tokens, int token_idx, Error* error, bool is_first_itteration, LabelNode* labelPtr);

int getOperandAmount(char* command);

int getAdressingMethodByOperandType(OperandType operand_type);

void convertToBase64(short num, char* result);

void createOperandBinaryWord(int L, LabelNode* labels, bool is_first_iteration, OperandType op_type_source, OperandType op_type_destination, char* operand1, char* operand2, int* memory_idx, short* memory, Error* error);

void freeLinkedList(CodeNode* head);

int getLine(char* line, Error* error, FILE* file, int num_line, bool* is_print);

void cleanLine(char* line, int length);

void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, int* pnum_tokens, char** tokens, bool* is_print, Error* error);

void macrosToValues(CodeNode** code, MacroNode** macros, char *tokens[], int* pnum_tokens, bool* is_print, Error* error);

void createFileWithMemoryDump(char* file_name, short* memory, int* memory_idx, int IC, int DC);

void* allocateMemory(size_t size, bool* is_print, Error* error);

bool handleError(Error* error, int num_line, bool* is_print);

/**
 * @brief checks if a line is a correct data line or not
 * 
 * @param tokens 
 * @param num_tokens 
 * @param label - a boolean that says if the 1st token is a label or not
 * @return true - if it is a data line and its correct
 * @return false - when its not a data line or an incorrect one 
 */
bool checkDataLine(char** tokens, int num_tokens, bool label, Error* error);

/**
 * @brief takes in a string and checks if its a legal number
 * 
 * @param word 
 * @return true if it is a number
 * @return false otherwise
 */
bool isNumber(char* word);

bool isString(char* string);

/**
 * @brief checks if the word is .data or .string , returns false if neither
 * 
 * @param word 
 * @return 4 - if its .extern 3 - if its a .entry 2 - if its a .string 1 if it is a .data 0 if it neither of them 
 */
DotType isDotType(char* word, Error* error);

LabelType getLabelType(char* label, LabelNode* LabelNode, Error* error);

void nextLine(CodeNode** temp_code, int* num_line);

/*void printTokens(char** tokens, int* num_tokens);*/

#endif