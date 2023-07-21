#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

CodeNode* preproccessor(char* file_name, Error* error) {
    CodeNode* code;
    MacroNode* macros = NULL;
    FILE* fptr;
    char** tokens;

    int num_tokens = 0;

    tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    if (*error != NO_ERROR) {
        *error = NO_ERROR;
        return NULL;
    }

    fptr = fopen(file_name, "r");
    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error);
        *error = NO_ERROR;
        return NULL;
    }

    code = createLinkedListFromFile(fptr, tokens, &num_tokens, error);
    if (*error != NO_ERROR) {
        *error = NO_ERROR;
        return NULL;
    }
    scanCodeForMacroDefinitions(&code, &macros, &num_tokens, tokens, error);
    if (*error != NO_ERROR) {
        *error = NO_ERROR;
        return NULL;
    }
    macrosToValues(&code, &macros, tokens, &num_tokens, error);
        if (*error != NO_ERROR) {
        *error = NO_ERROR;
        return NULL;
    }
    return code;
}

CodeNode* createLinkedListFromFile(FILE* fptr, char *tokens[], int* pnum_tokens, Error* error) {
    char buffer[MAX_LINE_LENGTH]; 
    CodeNode *head = NULL, *temp = NULL, *node = NULL;

    while(getLine(buffer, error, fptr)) {
        /*Create a new node*/
        node = (CodeNode*) allocateMemory(sizeof(CodeNode), error);

        if (*error != NO_ERROR) {
            return NULL;
        }
        /*printing the contents of the buffer, to see what's inside*/
        node->code_row = (char*) allocateMemory(strlen(buffer) + 1, error);

        if (*error != NO_ERROR) {
            *error = NO_ERROR;
            return NULL;
        }

        /* Copy the string from buffer to the new node*/
        strcpy(node->code_row, buffer);
        
        /* If this is the first node, it is the head of the list*/
        if(!head) {
            head = node;
        } else {
            /* Otherwise, add the new node to the end of the list*/
            temp->next = node;
        }
        
        /* Move the temporary pointer to the new node*/
        temp = node;
    }
    return head;
}

void freeLinkedList(CodeNode* head) {
    CodeNode* tmp;

    while (!head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int getLine(char* line, Error* error, FILE* fptr) {
    char x; /*current symbol in the input stream*/
    int i = 0;
    cleanLine(line, MAX_LINE_LENGTH);
    while ((x = fgetc(fptr)) != '\n' && x != EOF) {
        if (i == MAX_LINE_LENGTH) {
            *error = ERROR_MAXED_OUT_LINE_LENGTH;
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
        
        if (i != 0 && line[i-1] == ':' && x != ' ') {
            line[i++] = ' ';
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
        line[0] = '\n';
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

void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, int* pnum_tokens, char** tokens, Error* error) {
    MacroNode* new_macro_node;
    MacroNode* temp_macro_node;
    CodeNode* new_code_node;
    CodeNode* new_code_node_head;
    CodeNode* curr_code_node;

    temp_macro_node = NULL;
    curr_code_node = *code_node;
    while (curr_code_node) {
        tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, error);
        if (*error != NO_ERROR) {
            return;
        }
        if (*pnum_tokens == 2 && !strcmp(tokens[0], "mcro") ) {
            new_macro_node = (MacroNode*) allocateMemory(sizeof(MacroNode), error);
            new_macro_node->code_node = (CodeNode*) allocateMemory(sizeof(CodeNode), error);
            /*if (!validateVariableName(tokens[1])) {
                
            } else {
                
            }*/
            new_macro_node->macro_name = my_strdup(tokens[1]);

            curr_code_node = curr_code_node->next;
            tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, error);

            new_code_node = (CodeNode*) allocateMemory(sizeof(CodeNode), error);
            new_code_node_head = new_code_node;

            while(curr_code_node && strcmp(tokens[0], "endmcro")) {
                if (new_code_node->code_row) {
                    new_code_node->next = (CodeNode*) allocateMemory(sizeof(CodeNode), error);
                    new_code_node = new_code_node->next;
                }
                new_code_node->code_row = my_strdup(curr_code_node->code_row);
                curr_code_node = curr_code_node->next;
                tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, error);
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
    }
}

void macrosToValues(CodeNode **code, MacroNode **macros, char *tokens[], int *pnum_tokens, Error* error)
{
    /* Declare variables */
    MacroNode *current_macro;
    CodeNode *current_code;
    CodeNode *current_macro_code;
    CodeNode *prev_code;
    CodeNode *temp;

    bool macro_replaced = false; /* Flag to track if a macro is replaced */
    /*test comments*/
    /* Initialize variables */
    current_code = *code;
    prev_code = NULL;
    temp = NULL;

    while (current_code)
    {
        tokenizeInput(current_code->code_row, tokens, pnum_tokens, error);

        if (*pnum_tokens == 1)
        {
            current_macro = *macros;
            macro_replaced = false;

            while (current_macro)
            {
                if (!strcmp(current_macro->macro_name, tokens[0]))
                {
                    /* Replace the macro name with the code lines */
                    current_macro_code = current_macro->code_node;

                    while (current_macro_code)
                    {
                        /* Create a new code node and copy the code row */
                        CodeNode *new_code_node = (CodeNode *)allocateMemory(sizeof(CodeNode), error);
                        new_code_node->code_row = my_strdup(current_macro_code->code_row);

                        /* Append the new code node to the linked list */
                        if (prev_code)
                            prev_code->next = new_code_node;
                        else
                            *code = new_code_node;

                        prev_code = new_code_node;

                        current_macro_code = current_macro_code->next;
                    }

                    macro_replaced = true;
                }

                current_macro = current_macro->next;
            }

            /* Remove the current code node if a macro was replaced */
            if (macro_replaced)
            {
                if (prev_code)
                    prev_code->next = current_code->next;
                else
                    *code = current_code->next;

                free(current_code);
                current_code = prev_code;
            }
        }
        else if (*pnum_tokens == 2 && !strcmp(tokens[0], "mcro"))
        {
            while (true)
            {
                temp = current_code;
                current_code = current_code->next;
                free(temp);
                tokenizeInput(current_code->code_row, tokens, pnum_tokens, error);
                if (*pnum_tokens == 1 && !strcmp(tokens[0], "endmcro"))
                {
                    prev_code->next = current_code->next;
                    break;
                }
            }    
        }
        prev_code = current_code;
        current_code = current_code->next;
    }
}
