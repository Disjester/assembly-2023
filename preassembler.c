#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

CodeNode* preproccessor(char* file_name, bool* is_print, Error* error) {
    CodeNode* code = NULL;
    MacroNode* macros = NULL;
    FILE* fptr;
    char** tokens;
    int num_tokens = DEFAULT_VALUE;
    char full_path[MAX_FILE_NAME_WITH_EXTENSION];

    tokens = allocateMemory(MAX_TOKENS * sizeof(char *), is_print, error);
    allocateMemoryTokens(tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }

    cleanLine(full_path, MAX_FILE_NAME_WITH_EXTENSION);
    strcpy(full_path, file_name);
    strcat(full_path, ".as");

    fptr = fopen(full_path, "r");

    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error, DEFAULT_LINE_NUMER, is_print);
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    code = createLinkedListFromFile(fptr, tokens, &num_tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    
    scanCodeForMacroDefinitions(&code, &macros, &num_tokens, tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    
    insertMacrosToCode(&code, &macros, tokens, &num_tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    fclose(fptr);
    freeMemory(tokens, NULL, macros, NULL);
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
            freeMemory(tokens, code_node, NULL, NULL);
            return NULL;
        }
        /*printing the contents of the buffer, to see what's inside*/
        code_node->code_row = (char*) allocateMemory(strlen(buffer) + 1, is_print, error);
        if (*error != NO_ERROR) {
            freeMemory(tokens, code_node, NULL, NULL);
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

void insertMacrosToCode(CodeNode **code, MacroNode **macros, char *tokens[], int *pnum_tokens, bool* is_print, Error* error) {
    /* Declare variables */
    MacroNode *current_macro;
    CodeNode *current_code;
    CodeNode *current_macro_code;
    CodeNode *prev_code;
    CodeNode *temp;
    CodeNode *first_macro_code_node;
    CodeNode *latest_macro_code_node = NULL;
    
    bool      is_first_mcro_line = true;
    bool macro_found = false; /* Flag to track if a macro is found */
    bool first_connection = true;

    /* Initialize variables */
    current_code = *code;
    prev_code = (CodeNode *)allocateMemory(sizeof(CodeNode), is_print, error);
    prev_code->code_row = my_strdup(" ", is_print, error);
    temp = NULL;

    while (current_code) {
        tokenizeInput(current_code->code_row, tokens, pnum_tokens, is_print, error);
        if (*error == ERROR_MEMORY_ALLOCATION) return;
        
        if (*pnum_tokens == 2 && !strcmp(tokens[FIRST_WORD], "mcro")) {
            while (true) {
                temp = current_code;
                current_code = current_code->next;
                free(temp->code_row);
                free(temp);
                tokenizeInput(current_code->code_row, tokens, pnum_tokens, is_print, error);
                if (*error != NO_ERROR) return;
                /* if got to endmcro , link the code before mcro "mcro_name" . and after endmcro. 
                and free the current code line ( of endmcro )
                */
                if (*pnum_tokens == 1 && !strcmp(tokens[FIRST_WORD], "endmcro")) {
                    temp = current_code;
                    current_code = current_code->next;
                    prev_code->next = current_code;
                    free(temp->code_row);
                    free(temp);

                    break;
                }
                
            }
        }
            
        
        if (*pnum_tokens == 1) {
            tokenizeInput(current_code->code_row, tokens, pnum_tokens, is_print, error);
            current_macro = *macros;
            macro_found = false;

            /* going through all the macros */
            while (current_macro) {
                /* if found a mcro name like m1 */
                if (!strcmp(current_macro->macro_name, tokens[FIRST_WORD])) {
                    first_connection = true;
                    macro_found = true;

                    /* go throught all of the mcro code node, code lines 
                       and copy it to another code node
                    */
                    current_macro_code = current_macro->code_node;
                    while (current_macro_code)
                    {
                        if (is_first_mcro_line)
                        {
                            first_macro_code_node = (CodeNode *)allocateMemory(sizeof(CodeNode), is_print, error);
                            if (*error == ERROR_MEMORY_ALLOCATION) return;
                            first_macro_code_node->code_row = my_strdup(current_macro_code->code_row, is_print, error);
                            if (*error == ERROR_MEMORY_ALLOCATION) return;

                            is_first_mcro_line = false;
                            current_macro_code = current_macro_code->next;
                        }
                        else
                        {
                            latest_macro_code_node = (CodeNode *)allocateMemory(sizeof(CodeNode), is_print, error);
                            if (*error == ERROR_MEMORY_ALLOCATION) return;
                            latest_macro_code_node->next = NULL;
                            latest_macro_code_node->code_row = my_strdup(current_macro_code->code_row, is_print, error);
                            if (*error == ERROR_MEMORY_ALLOCATION) return;
                            if (first_connection)
                            {
                                first_macro_code_node->next = latest_macro_code_node;
                                first_connection = false;
                            }
                            if (current_macro_code->next)
                            {
                                latest_macro_code_node = latest_macro_code_node->next;
                            }
                            current_macro_code = current_macro_code->next;
                        }
                        
                    }
                }
                if (macro_found)
                {
                    break;
                }
                current_macro = current_macro->next;
            }

            /* Remove the current code node  ( line of text ) if a macro name was found */
            if (macro_found) {

                    /* add the macro code nodes to the main code */
                    if (latest_macro_code_node)
                    {
                        latest_macro_code_node->next = current_code->next;
                    }
                    else
                    {
                        first_macro_code_node->next = current_code->next;
                    }
                    
                    prev_code->next = first_macro_code_node;
                    
                    /* free the current code node ( code line of text ) */
                    temp = current_code;
                    current_code = current_code->next;
                    free(temp->code_row);
                    free(temp);
            }
        }
        prev_code = current_code;
        current_code = current_code->next;
    }
}
