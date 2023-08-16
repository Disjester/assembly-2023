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

void createOutputFiles (char* file_name, LabelNode* labels, short* memory, int* memory_idx, int IC, int DC, LabelNode* externals, bool* is_print, Error* error, int num_line);

void createBinaryWordByType(LabelNode* labels, OperandType op_type, char* operand, short* memory, int* memory_idx, bool is_first_iteration, Error* error, int num_line, bool* is_print);

void createFileWithLabelType(char* file_name, LabelNode* labels, LabelType label_type, Error* error);

void cleanMemory(short* memory);

void updateEntryLabels(LabelNode* labels, char** tokens, int num_tokens, int token_idx);

char* removeColon(char* str);

void insertNewLabel(LabelNode** labels, char* label_name, LabelType label_type, int* memory_idx, bool* is_print, Error* error, bool is_first_itteration_flag);

void firstIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode** labels, int* DC, int* IC, bool* is_print, Error* error);

void secondIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode* labels, int* DC, int* IC, Error* error, char* file_name, LabelNode* externals, bool* is_print);

void pushToMemory(int* memory_idx, short* memory, short memoryField, Error* error, int num_line, bool *is_print);

int getOperandsNumberByOpcode(short opcode);

/** 
 * Validates a variable name according to specific rules.
 *
 * @param name The variable name to validate.
 * @return true if the variable name is valid, false otherwise.
 */
bool validateVariableName (char *name);

/** 
 * Preprocesses the source file, handling macros and replacing them with their definitions.
 *
 * @param file_name The name of the source file.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 * @return A linked list of CodeNode representing the preprocessed code.
 */
CodeNode* preproccessor(char *file_name, bool* is_print, Error* error);

/** 
 * Tokenizes an input string and stores the tokens in an array.
 *
 * @param input The input string to tokenize.
 * @param tokens An array to store the tokens. ( needs to be allocated beforehand)
 * @param num_tokens Pointer to the number of tokens.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 */
void tokenizeInput(char *input, char **tokens, int *num_tokens, bool* is_print, Error* error);

/** 
 * Duplicates a string using dynamic memory allocation.
 *
 * @param str The string to duplicate.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 * @return A pointer to the duplicated string.
 */
char *my_strdup(const char *str, bool* is_print, Error* error);

/**
 * Creates a linked list of CodeNode from the source file.
 *
 * @param fptr Pointer to the source file.
 * @param tokens An array of strings to hold tokens.
 * @param pnum_tokens Pointer to the number of tokens.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 * @return A linked list of CodeNode representing the code from the file.
 */
CodeNode* createLinkedListFromFile(FILE* file, char *tokens[], int* pnum_tokens, bool* is_print, Error* error);

short createCommandBinaryWord(char** tokens, int num_tokens, int token_idx, Error* error, bool is_first_itteration, LabelNode* labelPtr);

int getOperandAmount(char* command);

int getAdressingMethodByOperandType(OperandType operand_type);

void convertToBase64(short num, char* result);

void createOperandBinaryWord(int L, LabelNode* labels, bool is_first_iteration, OperandType op_type_source, OperandType op_type_destination, char* operand1, char* operand2, int* memory_idx, short* memory, Error* error, int num_line, bool* is_print);

/**
 * Reads a line from the file and handles various cases.
 *
 * @param line Buffer to store the read line.
 * @param error Pointer to an Error variable for error handling.
 * @param fptr Pointer to the source file.
 * @param num_line The current line number.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @return The number of characters read.
 */
int getLine(char* line, Error* error, FILE* file, int num_line, bool* is_print);

/**
 * Cleans a line buffer by filling it with null characters.
 *
 * @param line The line buffer to be cleaned.
 * @param length The length of the line buffer.
 */
void cleanLine(char* line, int length);

/**
 * Scans the code for macro definitions and stores them in the macro list.
 *
 * @param code_node Pointer to the head of the code linked list.
 * @param macro_node Pointer to the head of the macro linked list.
 * @param pnum_tokens Pointer to the number of tokens.
 * @param tokens An array of strings containing tokens.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 */
void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, int* pnum_tokens, char** tokens, bool* is_print, Error* error);

/**
 * Replaces macro invocations with their definitions in the code.
 *
 * @param code Pointer to the head of the code linked list.
 * @param macros Pointer to the head of the macro linked list.
 * @param tokens An array of strings containing tokens.
 * @param pnum_tokens Pointer to the number of tokens.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 */
void macrosToValues(CodeNode** code, MacroNode** macros, char *tokens[], int* pnum_tokens, bool* is_print, Error* error);

void createFileWithMemoryDump(char* file_name, short* memory, int* memory_idx, int IC, int DC, Error* error,  int num_line, bool* is_print);

/** 
 * Allocates memory with error handling.
 *
 * @param size The size of memory to allocate.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable for error handling.
 * @return A pointer to the allocated memory.
 */
void* allocateMemory(size_t size, bool* is_print ,Error* error);

/** 
 * Handles and prints errors based on error codes.
 *
 * @param error Pointer to an Error variable containing the error code.
 * @param num_line The line number where the error occurred.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @return true if an error occurred, false otherwise.
 */
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

/** 
 * Frees memory allocated for tokens, code nodes, macro nodes, and label nodes.
 *
 * @param tokens An array of tokens.
 * @param code_node1 A pointer to a CodeNode.
 * @param code_node2 A pointer to another CodeNode.
 * @param code_node3 A pointer to a third CodeNode.
 * @param macro_node A pointer to a MacroNode.
 * @param label_node A pointer to a LabelNode.
 */
void freeMemory(char** tokens, CodeNode* code_node1, CodeNode* code_node2, CodeNode* code_node3, MacroNode* macro_node, LabelNode* label_node);

/** 
 * Recursively frees memory allocated for a linked list of CodeNodes.
 *
 * @param code_node A pointer to a CodeNode.
 */
void freeMemoryCodeNode(CodeNode* code_node);

/** 
 * Recursively frees memory allocated for a linked list of MacroNodes.
 *
 * @param macro_node A pointer to a MacroNode.
 */
void freeMemoryMacroNode(MacroNode* macro_node);

/** 
 * Recursively frees memory allocated for a linked list of LabelNodes.
 *
 * @param label_node A pointer to a LabelNode.
 */
void freeMemoryLabelNode(LabelNode* label_node);

void nextLine(CodeNode** temp_code, int* num_line);

bool checkExternalEntryLine(char** tokens, int num_tokens, Error* error, LabelNode** labels, LabelType label_type, bool is_first_itteration);

bool isDuplicatedLabel(LabelNode** labels, char* label_name, LabelType label_type, Error* error, bool is_first_itteration);

void allocateMemoryTokens(char** tokens, bool* is_print, Error* error);

/*void printTokens(char** tokens, int* num_tokens);*/

#endif