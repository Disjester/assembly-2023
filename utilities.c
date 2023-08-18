#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

static const char base64_chars[64] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

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
        strcpy(tokens[*num_tokens], token);  /* copies the token to the tokens array */
        if (*error == ERROR_MEMORY_ALLOCATION) return;
        (*num_tokens)++;
        token = strtok(NULL, " \r");
    }
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

/** 
 * Allocates memory to array of tokens
 * 
 * @param tokens A pointer to a LabelNode.
 * @param is_print Pointer to a boolean indicating whether to print error messages.
 * @param error Pointer to an Error variable containing the error code.
 */
void allocateMemoryTokens(char** tokens, bool* is_print, Error* error) {
    int i;

    for (i = 0; i < MAX_TOKENS; i++) {
        tokens[i] = allocateMemory(MAX_TOKEN_LENGTH * sizeof(char), is_print, error);
    }
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

void freeMemory(char** tokens, CodeNode* code_node, MacroNode* macro_node, LabelNode* label_node) {
    int i;

    if (tokens) {
        for (i = 0; i < MAX_TOKENS; i++) {
            if(tokens[i]) {
                free(tokens[i]);
            }
        }
        free(tokens);
    }
    freeMemoryCodeNode(code_node);
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

/** 
 * Recursively frees memory allocated for a linked list of LabelNodes.
 *
 * @param label_node A pointer to a LabelNode.
 */
void freeMemoryLabelNode(LabelNode* label_node) {
    if (label_node) {
        freeMemoryLabelNode(label_node->next);
    }
    if (!label_node) {
        return;
    }
    if(label_node->label_name) {
        free(label_node->label_name);
    }
    free(label_node);
}

void cleanMemory(short* memory) {
    int i;

    for (i = 0; i < MAX_MEMORY_SIZE; i++) {
        memory[i] = DEFAULT_ERROR_VALUE;
    }
}

char* removeColon(char* str) {
    str[strlen(str)-1] = '\0';
    return str;
}

void convertToBase64(short num, char* result) {
    result[0] = base64_chars[(num >> 6) & MASK64];
    result[1] = base64_chars[num & MASK64];
    result[2] = '\0';
}

int getLine(char* line, Error* error, FILE* fptr, int num_line, bool* is_print) {
    char x; /*current symbol in the input stream*/
    int i = 0;
    cleanLine(line, MAX_LINE_LENGTH);
    while ((x = fgetc(fptr)) != '\n' && x != EOF) {
        if (i == MAX_LINE_LENGTH) {
            *error = ERROR_MAXED_OUT_LINE_LENGTH;
            handleError(error, num_line, is_print);
            /*skipping to the next line*/
            while ((x = fgetc(fptr)) != '\n' && x != EOF) {
                continue;
            }
            return i;
        }
        /*substitution of whitespaces instead of tabs*/
        x = (x == '\t') ? ' ' : x;
        /*removing whitespaces at the beggining of the line*/
        if (i == 0 && x == ' ') {
            continue;
        }

        if (i != 0 && x == ',') {
            if (line[i-1] != ' ') {
                line[i++] = ' ';
            }
            line[i++] = x;
            line[i++] = ' ';
            continue;
        }
        
        /*removing of duplications of whitespaces*/
        if ((i != 0) && line[i-1] == ' ' && (x == ' ')) {
            continue;
        }
        /*putting a char to the string*/
        line[i++] = x;
    }
    /*The case where the line is empty*/
    if (i == 0 && x == '\n') {
        line[FIRST_CHARACTER] = '\0';
        return 1;
    }
    return i;
}

void cleanLine(char* line, int length) {
    int i;
    for (i = 0; i < length; i++) {
        line[i] = '\0';
    }
}

bool isNumber(char* word){
    int i = 0;
    int len = strlen(word);

    /* Check for a minus sign at the beginning
     Skip it if exists */
    if (word[i] == '-') i++;  
    
    for ( ; i < len; i++) {
        if (!isdigit(word[i])) {
            return false; /* not a number*/
        }
    }
    return true;
}

bool isString(char** tokens, int num_tokens, bool label) {
    /*an idea for taking in  tokens , and then going through all of the tokens characters. basically just add
    an outside loop. */
    int char_index = 0;
    int string_index = label + 1;
    bool quote = false; /* a quote flag, that checks if found the 2nd (") character */
    int len = 0;

    /* checks the 1st character to be a quote */
    if (tokens[string_index][char_index++] != '"') {
        return false;
    }
    for (; string_index < num_tokens; string_index++)
    {
        len = strlen(tokens[string_index]);
        for (; char_index < len; char_index++) {
            if (quote) return false;

            if (tokens[string_index][char_index] == '"') quote = true;
        }
        char_index = 0;
    }

    /* returns false if 2nd quote isn't the last character */
    return quote;
}
