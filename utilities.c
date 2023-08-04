#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void tokenizeInput(char *input, char **tokens, int *num_tokens, Error* error) {
    size_t length = strlen(input);
    char *token = NULL;
    char *temp = NULL;
    temp = allocateMemory((length + 1) * sizeof(char), error);
    if (*error == ERROR_MEMORY_ALLOCATION) return;
    strcpy(temp, input);  /* Copy input string into temp */

    token = strtok(temp, " ");
    *num_tokens = 0;
    while (token != NULL && *num_tokens < MAX_TOKENS) {
        tokens[*num_tokens] = my_strdup(token, error);  /* Duplicate and store token */
        if (*error == ERROR_MEMORY_ALLOCATION) return;

        (*num_tokens)++;
        token = strtok(NULL, " ");
    }
    /*printTokens(tokens, num_tokens);*/
    free(temp);
}

char *my_strdup(const char *str, Error* error) {
    size_t length = strlen(str);
    char *duplicate = allocateMemory(length + 1, error);  /* Allocate memory for the duplicate string*/
    if (*error == ERROR_MEMORY_ALLOCATION) return NULL;
    if (duplicate != NULL) {
        strcpy(duplicate, str);  /* Copy the string into the allocated memory*/
    }
    return duplicate;
}

void* allocateMemory(size_t size, Error* error) {
    void* ptr = calloc(1, size);
    if (!ptr) {
        *error = ERROR_MEMORY_ALLOCATION;
        handleError(error, DEFAULT_LINE_NUMER);
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

bool handleError(Error* error, int num_line) {
    switch (*error) {
        case ERROR_MEMORY_ALLOCATION:
            fprintf(stderr, "ERROR ON %d: MEMORY ALLOCATION FAILED\n", num_line);
            return true;
        case ERROR_FILE_HANDLE:
            fprintf(stderr, "ERROR ON %d: FILE COULDN'T BE OPENED OR DOESN'T EXIST\n", num_line);
            return true;
        case ERROR_ILLEGAL_MACRO_NAME:
            fprintf(stderr, "ERROR ON %d: ILLEGAL NAME\n", num_line);
            return true;
        case ERROR_DUPLICATED_MACRO_DEFINITION:
            fprintf(stderr, "ERROR ON %d: DUPLICATED MACRO DEFINITION\n", num_line);
            return true;
        case ERROR_EXTRANEOS_TEXT:
            fprintf(stderr, "ERROR ON %d: EXTRANEOS_TEXT\n", num_line);
            return true;
        case ERROR_ILLEGAL_COMMA:
            fprintf(stderr, "ERROR ON %d: ILLEGAL_COMMA\n", num_line);
            return true;
        case ERROR_ILLEGAL_LABEL_NAME:
            fprintf(stderr, "ERROR ON %d: ILLEGAL_LABEL_NAME\n", num_line);
            return true;
        case ERROR_ILLEGAL_OPERAND_TYPE:
            fprintf(stderr, "ERROR ON %d: ILLEGAL_OPERAND_TYPE\n", num_line);
            return true;
        case ERROR_INCORRECT_OPERAND_TYPE:
            fprintf(stderr, "ERROR ON %d: INCORRECT_OPERAND_TYPE\n", num_line);
            return true;
        case ERROR_MAXED_OUT_LINE_LENGTH:
            fprintf(stderr, "ERROR ON %d: MAXED_OUT_LINE_LENGTH\n", num_line);
            return true;
        case ERROR_MISSING_COMMA:
            fprintf(stderr, "ERROR ON %d: ERROR_MISSING_COMMA\n", num_line);
            return true;
        case ERROR_MISSING_DATA_ARGUMENT:
            fprintf(stderr, "ERROR ON %d: MISSING_DATA_ARGUMENT\n", num_line);
            return true;
        case ERROR_UNDEFINED_COMMAND:
            fprintf(stderr, "ERROR ON %d: UNDEFINED_COMMAND\n", num_line);
            return true;
        case ERROR_UNDEFINED_VARIABLE:
            fprintf(stderr, "ERROR ON %d: UNDEFINED_VARIABLE\n", num_line);
            return true;
        case ERROR_UNRECOGNIZED_LABEL:
            fprintf(stderr, "ERROR ON %d: UNRECOGNIZED_LABEL\n", num_line);
            return true;
        case ERROR_WRONG_AMOUNT_OF_OPERANDS:
            fprintf(stderr, "ERROR ON %d: WRONG_AMOUNT_OF_OPERANDS\n", num_line);
            return true;
        case ERROR_WRONG_ARGUMENT_FORMAT:
            fprintf(stderr, "ERROR ON %d: WRONG_ARGUMENT_FORMAT\n", num_line);
            return true;
        case ERROR_WRONG_NUM_OF_COMMAS:
            fprintf(stderr, "ERROR ON %d: WRONG_NUM_OF_COMMAS\n", num_line);
            return true;
        case ERROR_MISSING_VARIABLE:
            fprintf(stderr, "ERROR ON %d: ERROR_MISSING_VARIABLE\n", num_line);
            return true;
        case ERROR_MAXED_OUT_MEMORY:
            fprintf(stderr, "ERROR ON %d: MAXED OUT MEMMORY\n", num_line);
            return true;
        case ERROR_CODE_AFTER_STOP:
            fprintf(stderr, "ERROR ON %d: CODE LINE AFTER STOP\n", num_line);
            return true;
        default:
            return false;
    }
}

/*
void printTokens(char** tokens, int* num_tokens){
    int i = 0;
    for ( ; i < *num_tokens; i++)
    {
        printf("%s ",tokens[i]);
    }
    printf("\n");
}
*/