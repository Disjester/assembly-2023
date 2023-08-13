#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void tokenizeInput(char *input, char **tokens, int *num_tokens, bool* is_print, Error* error) {
    size_t length = strlen(input);
    char *token = NULL;
    char *temp = NULL;
    temp = allocateMemory((length + 1) * sizeof(char), is_print, error);
    if (*error == ERROR_MEMORY_ALLOCATION) return;
    strcpy(temp, input);  /* Copy input string into temp */
    
    token = strtok(temp, " \r");
    *num_tokens = 0;
    while (token != NULL && *num_tokens < MAX_TOKENS) {
        /*if (tokens[*num_tokens]) {
            free(tokens[*num_tokens]);
        }*/
        tokens[*num_tokens] = my_strdup(token, is_print, error);  /* Duplicate and store token */
        if (*error == ERROR_MEMORY_ALLOCATION) return;
        (*num_tokens)++;
        token = strtok(NULL, " \r");
    }

    /*printTokens(tokens, num_tokens);*/
    free(temp);
}

char *my_strdup(const char *str, bool* is_print, Error* error) {
    size_t length = strlen(str);
    char *duplicate = allocateMemory(length + 1, is_print, error);  /* Allocate memory for the duplicate string*/
    if (*error == ERROR_MEMORY_ALLOCATION) return NULL;
    if (duplicate != NULL) {
        strcpy(duplicate, str);  /* Copy the string into the allocated memory*/
    }
    return duplicate;
}

void* allocateMemory(size_t size, bool* is_print, Error* error) {
    void* ptr = calloc(1, size);
    /*printf("ALLOCATED MEMORY: %d (address), %lu (size)\n", ptr, size);*/
    if (!ptr) {
        *error = ERROR_MEMORY_ALLOCATION;
        handleError(error, DEFAULT_LINE_NUMER, is_print);
        return NULL;
    }
    return ptr;
}

bool validateVariableName (char *name) {
    int i;

    for (i = 0; i < strlen(name); i++) {
        if (i == 0 && !isalpha(name[i])) {
            return false;
        }
        if ((name[i] >= 0 && name[i] <= 57) || (name[i] >= 65 && name[i] <= 90) || (name[i] >= 97 && name[i] <= 122) || (name[i] == 45)) {
            continue;
        } else {
            return false;
        }
    }
    return true;
}

bool handleError(Error* error, int num_line, bool* is_print) {
    switch (*error) {
        case ERROR_MEMORY_ALLOCATION:
            fprintf(stderr, "ERROR ON %d: MEMORY ALLOCATION FAILED\n", num_line);
            break;
        case ERROR_FILE_HANDLE:
            fprintf(stderr, "ERROR ON %d: FILE COULDN'T BE OPENED OR DOESN'T EXIST\n", num_line);
            break;
        case ERROR_ILLEGAL_MACRO_NAME:
            fprintf(stderr, "ERROR ON %d: ILLEGAL NAME\n", num_line);
            break;
        case ERROR_DUPLICATED_MACRO_DEFINITION:
            fprintf(stderr, "ERROR ON %d: DUPLICATED MACRO DEFINITION\n", num_line);
            break;
        case ERROR_EXTRANEOS_TEXT:
            fprintf(stderr, "ERROR ON %d: EXTRANEOS TEXT\n", num_line);
            break;
        case ERROR_ILLEGAL_COMMA:
            fprintf(stderr, "ERROR ON %d: ILLEGAL COMMA\n", num_line);
            break;
        case ERROR_ILLEGAL_LABEL_NAME:
            fprintf(stderr, "ERROR ON %d: ILLEGAL LABEL NAME\n", num_line);
            break;
        case ERROR_ILLEGAL_OPERAND_TYPE:
            fprintf(stderr, "ERROR ON %d: ILLEGAL OPERAND TYPE\n", num_line);
            break;
        case ERROR_INCORRECT_OPERAND_TYPE:
            fprintf(stderr, "ERROR ON %d: INCORRECT OPERAND TYPE\n", num_line);
            break;
        case ERROR_MAXED_OUT_LINE_LENGTH:
            fprintf(stderr, "ERROR ON %d: MAXED OUT LINE LENGTH\n", num_line);
            break;
        case ERROR_MISSING_COMMA:
            fprintf(stderr, "ERROR ON %d: ERROR MISSING COMMA\n", num_line);
            break;
        case ERROR_MISSING_DATA_ARGUMENT:
            fprintf(stderr, "ERROR ON %d: MISSING DATA ARGUMENT\n", num_line);
            break;
        case ERROR_UNDEFINED_COMMAND:
            fprintf(stderr, "ERROR ON %d: UNDEFINED COMMAND\n", num_line);
            break;
        case ERROR_UNDEFINED_VARIABLE:
            fprintf(stderr, "ERROR ON %d: UNDEFINED VARIABLE\n", num_line);
            break;
        case ERROR_UNRECOGNIZED_LABEL:
            fprintf(stderr, "ERROR ON %d: UNRECOGNIZED LABEL\n", num_line);
            break;
        case ERROR_WRONG_AMOUNT_OF_OPERANDS:
            fprintf(stderr, "ERROR ON %d: WRONG AMOUNT OF OPERANDS\n", num_line);
            break;
        case ERROR_WRONG_ARGUMENT_FORMAT:
            fprintf(stderr, "ERROR ON %d: WRONG ARGUMENT FORMAT\n", num_line);
            break;
        case ERROR_WRONG_NUM_OF_COMMAS:
            fprintf(stderr, "ERROR ON %d: WRONG NUM OF COMMAS\n", num_line);
            break;
        case ERROR_MISSING_VARIABLE:
            fprintf(stderr, "ERROR ON %d: ERROR MISSING VARIABLE\n", num_line);
            break;
        case ERROR_MAXED_OUT_MEMORY:
            fprintf(stderr, "ERROR ON %d: MAXED OUT MEMMORY\n", num_line);
            break;
        case ERROR_CODE_AFTER_STOP:
            fprintf(stderr, "ERROR ON %d: CODE LINE AFTER STOP\n", num_line);
            break;
        case ERROR_NOT_ENOUGH_ARGUMENTS:
            fprintf(stderr, "ERROR ON %d: NOT ENOUGH ARGUMENTS\n", num_line);
            break;
        case ERROR_NO_STOP_COMMAND:
            fprintf(stderr, "ERROR        NO   STOP COMMAND\n");
            break;
        case ERROR_DUPLICATE_LABEL:
            fprintf(stderr, "ERROR ON %d: DUPLICATED LABEL\n", num_line);
        case NO_ERROR:
            break;
    }
    if (*error != NO_ERROR) {
        *is_print = false;
        return true;
        
    } else {
        return false;
    }
}

void freeMemory(char** tokens, CodeNode* code_node1, CodeNode* code_node2, CodeNode* code_node3, MacroNode* macro_node, LabelNode* label_node) {
    int i;

    /*for (i = 0; i < MAX_TOKENS; i++) {
        if (tokens[i]) {
            free(tokens[i]);
        }
    }*/

    /*if (tokens) {
        for (i = 0; i < MAX_TOKENS; i++) {
            if(tokens[i]) {
                tokens[i] = NULL;
            }
            
        }
        free(tokens);
    }*/
    printf("FREE\n");
    freeMemoryCodeNode(code_node1);
    freeMemoryCodeNode(code_node2);
    freeMemoryCodeNode(code_node3);

    freeMemoryMacroNode(macro_node);

    freeMemoryLabelNode(label_node);
}

void freeMemoryCodeNode(CodeNode* code_node) {
    if (code_node) {
        freeMemoryCodeNode(code_node->next);
    }
    if (!code_node) {
        return;
    }
    free(code_node->code_row);
    free(code_node);
}

void freeMemoryMacroNode(MacroNode* macro_node) {
    if (macro_node) {
        freeMemoryMacroNode(macro_node->next);
    }
    if (!macro_node) {
        return;
    }
    freeMemoryCodeNode(macro_node->code_node);
    free(macro_node->macro_name);
    free(macro_node);
}

void freeMemoryLabelNode(LabelNode* label_node) {
    if (label_node) {
        freeMemoryLabelNode(label_node->next);
    }
    if (!label_node) {
        return;
    }
    free(label_node->label_name);
    free(label_node);
}