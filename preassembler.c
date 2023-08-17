#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

CodeNode* preproccessor(char* file_name, bool* is_print, Error* error) {
    CodeNode* code;
    MacroNode* macros = NULL;
    FILE* fptr;
    char** tokens;
    int num_tokens = DEFAULT_VALUE;

    tokens = allocateMemory(MAX_TOKENS * sizeof(char *), is_print, error);
    allocateMemoryTokens(tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, NULL, NULL, macros, NULL);
        return NULL;
    }

    fptr = fopen(file_name, "r");
    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error, DEFAULT_LINE_NUMER, is_print);
        freeMemory(tokens, code, NULL, NULL, macros, NULL);
        return NULL;
    }
    code = createLinkedListFromFile(fptr, tokens, &num_tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, NULL, NULL, macros, NULL);
        return NULL;
    }
    
    scanCodeForMacroDefinitions(&code, &macros, &num_tokens, tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, NULL, NULL, macros, NULL);
        return NULL;
    }
    
    macrosToValues(&code, &macros, tokens, &num_tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, NULL, NULL, macros, NULL);
        return NULL;
    }
    fclose(fptr);
    freeMemory(tokens, NULL, NULL, NULL, macros, NULL);
    return code;
}

CodeNode* createLinkedListFromFile(FILE* fptr, char *tokens[], int* pnum_tokens, bool* is_print, Error* error) {
    char buffer[MAX_LINE_LENGTH]; 
    CodeNode *head = NULL, *temp = NULL, *code_node = NULL;
    int num_line = STARTING_LINE;

    while(getLine(buffer, error, fptr, num_line, is_print)) {
        if (*error == ERROR_MAXED_OUT_LINE_LENGTH) {
            num_line++;
            handleError(error, num_line, is_print);
            *error = NO_ERROR;
            continue;
        }
        /*Create a new node*/
        code_node = (CodeNode*) allocateMemory(sizeof(CodeNode), is_print, error);
        if (*error != NO_ERROR) {
            freeMemory(tokens, code_node, NULL, NULL, NULL, NULL);
            return NULL;
        }
        /*printing the contents of the buffer, to see what's inside*/
        code_node->code_row = (char*) allocateMemory(strlen(buffer) + 1, is_print, error);
        if (*error != NO_ERROR) {
            freeMemory(tokens, code_node, NULL, NULL, NULL, NULL);
            return NULL;
        }
        /* Copy the string from buffer to the new node*/
        strcpy(code_node->code_row, buffer);
        
        /* If this is the first node, it is the head of the list*/
        if(!head) {
            head = code_node;
        } else {
            /* Otherwise, add the new node to the end of the list*/
            temp->next = code_node;
        }
        
        /* Move the temporary pointer to the new node*/
        cleanLine(buffer, MAX_LINE_LENGTH);
        temp = code_node;
        num_line++;
    }
    return head;
}

int getLine(char* line, Error* error, FILE* fptr, int num_line, bool* is_print) {
    char x; /*current symbol in the input stream*/
    int i = DEFAULT_VALUE;
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
        if (i == DEFAULT_VALUE && x == ' ') {
            continue;
        }

        if (i != DEFAULT_VALUE && x == ',') {
            if (line[i-1] != ' ') {
                line[i++] = ' ';
            }
            line[i++] = x;
            line[i++] = ' ';
            continue;
        }
        
        if (i != DEFAULT_VALUE && line[i-1] == ':' && x != ' ') {
            line[i++] = ' ';
        }
        /*removing of duplications of whitespaces*/
        if ((i != DEFAULT_VALUE) && line[i-1] == ' ' && (x == ' ')) {
            continue;
        }
        /*putting a char to the string*/
        line[i++] = x;
    }
    /*The case where the line is empty*/
    if (i == DEFAULT_VALUE && x == '\n') {
        line[FIRST_CHARACTER] = '\0';
        return 1;
    }
    return i;
}

void cleanLine(char* line, int length) {
    int i;
    for (i = DEFAULT_VALUE; i < length; i++) {
        line[i] = '\0';
    }
}

void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, int* pnum_tokens, char** tokens, bool* is_print, Error* error) {
    MacroNode* new_macro_node;
    MacroNode* temp_macro_node;
    MacroNode* test_macro_node;
    CodeNode* new_code_node;
    CodeNode* new_code_node_head;
    CodeNode* curr_code_node;
    int num_line = STARTING_LINE;

    temp_macro_node = NULL;
    curr_code_node = *code_node;
    while (curr_code_node) {
        tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, is_print, error);
        if (*error == ERROR_MEMORY_ALLOCATION) return;

        if (*pnum_tokens == 2 && !strcmp(tokens[FIRST_WORD], "mcro") ) {
            if (!isLabel(tokens[SECOND_WORD], false) || checkCommand(tokens[SECOND_WORD]) != DEFAULT_ERROR_VALUE) {
                
                while (strcmp(tokens[FIRST_WORD], "endmcro")) {
                    curr_code_node = curr_code_node->next;
                    num_line++;
                    tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, is_print, error);
                    if (*error == ERROR_MEMORY_ALLOCATION) return;
                }
                curr_code_node = curr_code_node->next;
                num_line++;
                *error = ERROR_ILLEGAL_MACRO_NAME;
                handleError(error, num_line, is_print);
                continue;
            }
            test_macro_node = *macro_node;
            while (test_macro_node) {
                if (!strcmp(tokens[SECOND_WORD], test_macro_node->macro_name)) {
                    *error = ERROR_DUPLICATED_MACRO_DEFINITION;
                    handleError(error, num_line, is_print);
                }
                test_macro_node = test_macro_node->next;
            }

            new_macro_node = (MacroNode*) allocateMemory(sizeof(MacroNode), is_print, error);
            if (*error == ERROR_MEMORY_ALLOCATION) return;

            /*new_macro_node->code_node = (CodeNode*) allocateMemory(sizeof(CodeNode), is_print, error);
            if (*error == ERROR_MEMORY_ALLOCATION) return;*/

            new_macro_node->macro_name = my_strdup(tokens[SECOND_WORD], is_print, error);
            if (*error == ERROR_MEMORY_ALLOCATION) return;

            curr_code_node = curr_code_node->next;
            num_line++;
            tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, is_print, error);
            if (*error == ERROR_MEMORY_ALLOCATION) return;

            new_code_node = (CodeNode*) allocateMemory(sizeof(CodeNode), is_print, error);
            if (*error == ERROR_MEMORY_ALLOCATION) return;
            
            new_code_node_head = new_code_node;
            while(curr_code_node && strcmp(tokens[FIRST_WORD], "endmcro")) {
                if (new_code_node->code_row) {
                    new_code_node->next = (CodeNode*) allocateMemory(sizeof(CodeNode), is_print, error);
                    if (*error == ERROR_MEMORY_ALLOCATION) return;
                    new_code_node = new_code_node->next;
                }
                new_code_node->code_row = my_strdup(curr_code_node->code_row, is_print, error);
                if (*error == ERROR_MEMORY_ALLOCATION) return;

                curr_code_node = curr_code_node->next;
                num_line++;
                tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, is_print, error);
                if (*error == ERROR_MEMORY_ALLOCATION) return;
            }
            new_macro_node->code_node = new_code_node_head;
            if (temp_macro_node) {
                while (temp_macro_node->next) {
                    temp_macro_node = temp_macro_node->next;
                }
                temp_macro_node->next = new_macro_node;
                temp_macro_node = temp_macro_node->next;
            } else {
                *macro_node = new_macro_node;
                temp_macro_node = *macro_node;
            }
        }
        curr_code_node = curr_code_node->next;
        num_line++;
    }
}

void macrosToValues(CodeNode **code, MacroNode **macros, char *tokens[], int *pnum_tokens, bool* is_print, Error* error) {
    /* Declare variables */
    MacroNode *current_macro;
    CodeNode *current_code;
    CodeNode *current_macro_code;
    CodeNode *prev_code;
    CodeNode *temp;
    CodeNode *endmacro_node;
    bool      is_endmcro = false;

    bool macro_replaced = false; /* Flag to track if a macro is replaced */

    /* Initialize variables */
    current_code = *code;
    prev_code = NULL;
    temp = NULL;

    while (current_code) {
        tokenizeInput(current_code->code_row, tokens, pnum_tokens, is_print, error);
        if (*error == ERROR_MEMORY_ALLOCATION) return;

        if (*pnum_tokens == 1) {
            current_macro = *macros;
            macro_replaced = false;

            while (current_macro) {
                if (!strcmp(current_macro->macro_name, tokens[FIRST_WORD])) {
                    /* Replace the macro name with the code lines */
                    current_macro_code = current_macro->code_node;

                    while (current_macro_code)
                    {
                        /* Create a new code node and copy the code row */
                        CodeNode *new_code_node = (CodeNode *)allocateMemory(sizeof(CodeNode), is_print, error);
                        if (*error == ERROR_MEMORY_ALLOCATION) return;
                        new_code_node->code_row = my_strdup(current_macro_code->code_row, is_print, error);
                        if (*error == ERROR_MEMORY_ALLOCATION) return;
                        /* Append the new code node to the linked list */
                        if (prev_code) {
                            prev_code->next = new_code_node;
                        }
                        else {
                            *code = new_code_node;
                        }
                        prev_code = new_code_node;

                        current_macro_code = current_macro_code->next;
                    }
                    macro_replaced = true;
                }
                current_macro = current_macro->next;
            }
            
            /* Remove the current code node if a macro was replaced */
            if (macro_replaced) {
                if (prev_code)
                    prev_code->next = current_code->next;
                else
                    *code = current_code->next;
                free(current_code->code_row);
                free(current_code);
                current_code = prev_code;
            }
        }
        else if (*pnum_tokens == 2 && !strcmp(tokens[FIRST_WORD], "mcro")) {
            while (true) {
                temp = current_code;
                current_code = current_code->next;
                free(temp->code_row);
                free(temp);
                tokenizeInput(current_code->code_row, tokens, pnum_tokens, is_print, error);
                if (*error != NO_ERROR) return;

                if (*pnum_tokens == 1 && !strcmp(tokens[FIRST_WORD], "endmcro")) {
                    temp = current_code->next;
                    prev_code->next = current_code->next;
                    endmacro_node = current_code;
                    is_endmcro = true;
                    break;
                }
            }
        }
        prev_code = current_code;
        current_code = current_code->next;
        if (is_endmcro && endmacro_node) {
            free(endmacro_node->code_row);
            free(endmacro_node);
            is_endmcro = false;
        }
    }
}
