#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "constants.h"
#include "structures.h"
#include <stdio.h>

/*PREASSEMBLER SECTION*/

/**
 * Creates a linked list of CodeNode from the source file.
 *
 * @param fptr Pointer to the source file.
 * @param tokens An array of strings to hold tokens.
 * @param pnum_tokens Pointer to the number of tokens.
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 * @return A linked list of CodeNode representing the code from the file.
 */
CodeNode* createLinkedListFromFile(FILE* file, char *tokens[], int* pnum_tokens, bool* is_print, Error* error);

/** 
 * Preprocesses the source file, handling macros and replacing them with their definitions.
 *
 * @param file_name The name of the source file.
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 * @return A linked list of CodeNode representing the preprocessed code.
 */
CodeNode* preproccessor(char *file_name, bool* is_print, Error* error);

/**
 * Scans the code for macro definitions and stores them in the macro list.
 *
 * @param code_node Pointer to the head of the code linked list.
 * @param macro_node Pointer to the head of the macro linked list.
 * @param pnum_tokens Pointer to the number of tokens.
 * @param tokens An array of strings containing tokens.
 * @param is_print Pointer to a boolean indicating whether to create output files
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
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 */
void insertMacrosToCode(CodeNode** code, MacroNode** macros, char *tokens[], int* pnum_tokens, bool* is_print, Error* error);





/*ASSEMBLER SECTION*/

/**
 * @brief takes a line of code with command, and checks if its legal
 * 
 * @param tokens 
 * @param num_tokens 
 * @param label - Is the line starts with a label ? 
 * @return int  - > returns L = the number of memory words in the command line , -1 otherwise
 */
int checkCommandLine(char** tokens, int num_tokens, bool label, LabelNode* LabelPtr, Error* error, bool is_first_iteration, bool* stop_flag, bool* is_print, int num_line);

/**
 * @brief returns opcode of a command if it is, otherwise returns -1
 * 
 * @param word 
 * @return opcode of the operation or -1 if doesn't exist
 */
short checkCommand(char* word);

OperandType checkOperand(char* operand, LabelNode* LabelPtr, Error* error, bool is_first_iteration);
/**
 * @brief checks if the string is a label
 * 
 * @param word - string that should be checked
 * @param colon - if the end of label has to have ":" - put true, otherwise false
 * @return boolean that indicates whether the word is a legal label name
 */
bool isLabel(char* word, bool colon);

/**
 * @brief moves data from data_memory array , to global memmory.
 * 
 * @param data_memory  the place where data information is stored , points to the latest free memory slot
 * @param data_memory_idx index in the data array memory
 * @param memory  global memory array
 * @param memory_idx index in the global  memory array , points to the latest free memory slot
 * @param error pointer to error, for handlaing errors
 */
void moveDataToMemory(short* data_memory, int* data_memory_idx, short* memory, int* memory_idx, Error* error);

/**
 * @brief Create a Output Files object
 * 
 * @param file_name takes the assembly code file name
 * @param labels pointer to the labels linked list
 * @param memory pointer to the global memory array
 * @param memory_idx  index in the global  memory array , points to the latest free memory slot
 * @param IC Instruction counter
 * @param DC Data Counter
 * @param externals pointer to externals linked list
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error pointer to error, for handlaing errors
 * @param num_line the line number on which the function is performed ( part of the error handling mechanism )
 */
void createOutputFiles (char* file_name, LabelNode* labels, short* memory, int* memory_idx, int IC, int DC, LabelNode* externals, bool* is_print, Error* error, int num_line);


/**
 * @brief Creates a binary word based on the operand type and stores it in memory.
 * 
 * @param labels Pointer to the labels linked list.
 * @param op_type The type of operand.
 * @param operand The operand value.
 * @param memory Global memory array.
 * @param memory_idx Index in the global memory array.
 * @param is_first_iteration Flag indicating the first iteration.
 * @param error Pointer to an Error variable for error handling.
 * @param num_line The current line number.
 * @param is_print Pointer to a boolean indicating whether to create output files
 */
void createBinaryWordByType(LabelNode* labels, OperandType op_type, char* operand, short* memory, int* memory_idx, bool is_first_iteration, Error* error, int num_line, bool* is_print);

/**
 * @brief Creates an output file with the specified label type (entry or extern).
 * 
 * @param file_name The name of the output file.
 * @param labels Pointer to the labels linked list.
 * @param label_type The type of label (entry or extern).
 * @param error Pointer to an Error variable for error handling.
 */
void createFileWithLabelType(char* file_name, LabelNode* labels, LabelType label_type, bool* is_print, Error* error);

/**
 * @brief Updates entry labels with their corresponding memory addresses.
 * 
 * @param labels Pointer to the labels linked list.
 * @param tokens An array of tokens.
 * @param num_tokens The number of tokens.
 * @param token_idx The index of the token.
 */
void updateEntryLabels(LabelNode* labels, char** tokens, int num_tokens, int token_idx);

/**
 * @brief Inserts a new label into the label list.
 * 
 * @param labels Pointer to the labels linked list.
 * @param label_name The name of the label.
 * @param label_type The type of the label.
 * @param memory_idx Index in the global memory array.
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 * @param is_first_itteration_flag Flag indicating the first iteration.
 */
void insertNewLabel(LabelNode** labels, char* label_name, LabelType label_type, int* memory_idx, bool* is_print, Error* error, bool is_first_itteration_flag);

/**
 * @brief Performs the first iteration of the assembler.
 * 
 * @param memory Global memory array.
 * @param memory_idx Index in the global memory array.
 * @param code Pointer to the code linked list.
 * @param labels Pointer to the labels linked list.
 * @param DC Data Counter.
 * @param IC Instruction counter.
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 */
void firstIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode** labels, int* DC, int* IC, bool* is_print, Error* error);

/**
 * @brief Performs the second iteration of the assembler.
 * 
 * @param memory Global memory array.
 * @param memory_idx Index in the global memory array.
 * @param code Pointer to the code linked list.
 * @param labels Pointer to the labels linked list.
 * @param DC Data Counter.
 * @param IC Instruction counter.
 * @param error Pointer to an Error variable for error handling.
 * @param file_name The name of the output file.
 * @param externals Pointer to externals linked list.
 * @param is_print Pointer to a boolean indicating whether to create output files
 */
void secondIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode* labels, int* DC, int* IC, Error* error, char* file_name, LabelNode* externals, bool* is_print);

/**
 * @brief Pushes a memory field value into memory.
 * 
 * @param memory_idx Index in the global memory array.
 * @param memory Global memory array.
 * @param memoryField The memory field value to push.
 * @param error Pointer to an Error variable for error handling.
 * @param num_line The current line number.
 * @param is_print Pointer to a boolean indicating whether to create output files
 */
void pushToMemory(int* memory_idx, short* memory, short memoryField, Error* error, int num_line, bool* is_print);

/**
 * @brief Gets the number of operands for a command based on its opcode.
 * 
 * @param opcode The opcode of the command.
 * @return int The number of operands.
 */
int getOperandsNumberByOpcode(short opcode);

/**
 * Validates a variable name according to specific rules.
 *
 * @param name The variable name to validate.
 * @return true if the variable name is valid, false otherwise.
 */
bool validateVariableName (char *name);

/**
 * @brief Creates the binary word for a command and its operands.
 * 
 * @param tokens An array of tokens.
 * @param num_tokens The number of tokens.
 * @param token_idx The index of the current token.
 * @param error Pointer to an Error variable for error handling.
 * @param is_first_itteration Flag indicating the first iteration.
 * @param labelPtr Pointer to the labels linked list.
 * @return short The generated binary word.
 */
short createCommandBinaryWord(char** tokens, int num_tokens, int token_idx, Error* error, bool is_first_itteration, LabelNode* labelPtr);

/**
 * @brief Gets the number of operands for a command based on its mnemonic.
 * 
 * @param command The mnemonic of the command.
 * @return int The number of operands.
 */
int getOperandAmount(char* command);

/**
 * @brief Gets the addressing method value based on the operand type.
 * 
 * @param operand_type The type of operand.
 * @return int The addressing method value.
 */
int getAdressingMethodByOperandType(OperandType operand_type);

/**
 * @brief Creates the binary word for an operand based on its type.
 * 
 * @param L The L value (number of memory words) for the operand.
 * @param labels Pointer to the labels linked list.
 * @param is_first_iteration Flag indicating the first iteration.
 * @param op_type_source The source operand type.
 * @param op_type_destination The destination operand type.
 * @param operand1 The source operand value.
 * @param operand2 The destination operand value.
 * @param memory_idx Index in the global memory array.
 * @param memory Global memory array.
 * @param error Pointer to an Error variable for error handling.
 * @param num_line The current line number.
 * @param is_print Pointer to a boolean indicating whether to create output files
 */
void createOperandBinaryWord(int L, LabelNode* labels, bool is_first_iteration, OperandType op_type_source, OperandType op_type_destination, char* operand1, char* operand2, int* memory_idx, short* memory, Error* error, int num_line, bool* is_print);

/**
 * @brief Creates a memory dump file with the contents of the memory array.
 * 
 * @param file_name The name of the output file.
 * @param memory Pointer to the global memory array.
 * @param memory_idx Index in the global memory array.
 * @param IC Instruction counter.
 * @param DC Data counter.
 * @param error Pointer to an Error variable for error handling.
 * @param num_line The current line number.
 * @param is_print Indicates whether to create output files.
 */
void createFileWithMemoryDump(char* file_name, short* memory, int* memory_idx, int IC, int DC, Error* error, int num_line, bool* is_print);

/**
 * @brief checks if a line is a correct data line or not
 * 
 * @param tokens 
 * @param num_tokens 
 * @param isLabel - a boolean that says if the 1st token is a label or not
 * @return true - if it is a data line and its correct
 * @return false - when its not a data line or an incorrect one 
 */
bool checkDataLine(char** tokens, int num_tokens, bool isLabel, Error* error);

/**
 * @brief Determines the type of a dot command (e.g., .data, .string, .entry, .extern).
 * 
 * @param word The dot command string.
 * @param error Pointer to an Error variable for error handling.
 * @return The corresponding DotType value:
 *         - DOT_EXTERN if it's ".extern"
 *         - DOT_ENTRY if it's ".entry"
 *         - DOT_STRING if it's ".string"
 *         - DOT_DATA if it's ".data"
 *         - DOT_OTHER if it's none of the above.
 */
DotType getDotType(char* word, Error* error);

/**
 * @brief Determines the type of a label (external, entry, code) based on its name.
 * 
 * @param label The label name to be checked.
 * @param labels Pointer to the head of the LabelNode linked list.
 * @param error Pointer to an Error variable for error handling.
 * @return The corresponding LabelType value:
 *         - LABEL_EXTERNAL if it's an external label
 *         - LABEL_ENTRY if it's an entry label
 *         - LABEL_CODE if it's a regular label
 *         - LABEL_OTHER if it's none of the above.
 */
LabelType getLabelType(char* label, LabelNode* labels, Error* error);

/**
 * @brief Moves pointer to the next code line and increments num_line counter by 1
 * 
 * @param temp_code Pointer to the current code node
 * @param num_line Pointer to the current number of line
 */
void moveToNextCodeLine(CodeNode** temp_code, int* num_line);

/**
 * @brief checks if a line is a correct .extern or .entry line or not
 * 
 * @param tokens - array of tokens
 * @param num_tokens - number of tokens
 * @param error - Pointer to an Error variable for error handling.
 * @param labels - Array of labels
 * @param label_type - Type of the 
 * @param is_first_iteration - true if the function called from the first iteration
 * @return true - if it is a correct line
 * @return false - if it is a incorrect line
 */
bool checkExternalEntryLine(char** tokens, int num_tokens, Error* error, LabelNode** labels, LabelType label_type, bool is_first_itteration);

/**
 * @brief Determines if the label already exists in labels
 * 
 * @param labels Pointer to the head of the LabelNode linked list.
 * @param label_name array that stores name of the label
 * @param label_type stores type of the label
 * @param error Pointer to an Error variable for error handling.
 * @param is_first_itteration flag if the function is called from the first iteration
 * @return The corresponding LabelType value:
 *         - LABEL_EXTERNAL if it's an external label
 *         - LABEL_ENTRY if it's an entry label
 *         - LABEL_CODE if it's a regular label
 *         - LABEL_OTHER if it's none of the above.
 */
bool isDuplicatedLabel(LabelNode** labels, char* label_name, LabelType label_type, Error* error, bool is_first_itteration);

void createCodeFileWithoutMacros(char* file_name, CodeNode* code, bool* is_print, Error* error);





/*UTILITIES SECTION*/

/**
 * @brief Clears the memory by setting it to zero.
 * 
 * @param memory Global memory array.
 */
void cleanMemory(short* memory);

/**
 * @brief Removes a colon from a string at the last index.
 * 
 * @param str The string to process.
 * @return char* The processed string.
 */
char* removeColon(char* str);

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
 * @brief Converts a short integer to a base64 representation.
 * 
 * @param num The integer to convert.
 * @param result The output buffer to store the base64 representation.
 */
void convertToBase64(short num, char* result);

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
 * @brief Checks if a string represents a valid integer number.
 * 
 * @param word The string to be checked.
 * @return true if the string is a valid number, false otherwise.
 */
bool isNumber(char* word);

/**
 * @brief Checks if a string represents a valid string (enclosed in double quotes).
 * 
 * @param string The string to be checked.
 * @return true if the string is a valid string, false otherwise.
 */
bool isString( char** tokens, int num_tokens, bool label);


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
void freeMemory(char** tokens, CodeNode* code_node, MacroNode* macro_node, LabelNode* label_node);

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

void allocateMemoryTokens(char** tokens, bool* is_print, Error* error);


#endif