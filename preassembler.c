#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

/** 
 * Preprocesses the source file, handling macros and replacing them with their definitions.
 *
 * @param file_name The name of the source file.
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 * @return A linked list of CodeNode representing the preprocessed code.
 */
CodeNode* preproccessor(char* file_name, bool* is_print, Error* error) {
    /* Variable declarations */
    CodeNode* code = NULL; /* Linked list to hold preprocessed code */
    MacroNode* macros = NULL; /* Linked list to hold macro definitions */
    FILE* fptr; /* File pointer for reading the source file */
    char** tokens; /* Array of strings to hold tokens */
    int num_tokens = DEFAULT_VALUE; /* Number of tokens in a line of code */
    char full_path[MAX_FILE_NAME_WITH_EXTENSION]; /* Full path of the source file */

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

    /* if the file can't be opened , exit the preprocessor function */
    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error, DEFAULT_LINE_NUMER, is_print);
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }

    /* create linked list of code nodes from the assembly file (split the file into lines of code and save them )*/
    code = createLinkedListFromFile(fptr, tokens, &num_tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    
    /* look for macro defenitions and add them into a macro nodes - linked list */
    scanCodeForMacroDefinitions(&code, &macros, &num_tokens, tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    
    /* clear all the macro declaration lines of code, and switch all of the macro names with their declarations 
    ( all of the lines of code they contain )*/
    insertMacrosToCode(&code, &macros, tokens, &num_tokens, is_print, error);
    if (*error != NO_ERROR) {
        freeMemory(tokens, code, macros, NULL);
        return NULL;
    }
    fclose(fptr);
    freeMemory(tokens, NULL, macros, NULL);
    return code;
}

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
CodeNode* createLinkedListFromFile(FILE* fptr, char *tokens[], int* pnum_tokens, bool* is_print, Error* error) {
    char buffer[MAX_LINE_LENGTH]; 
    CodeNode *head = NULL, *temp = NULL, *code_node = NULL; /* declaring code node pointers to the first line of code (head of linked list)
                                                            temporary code node to add nodes into the linked list,
                                                            the original code node that is used to create a linked list 
                                                            and a new assembly file after all of the changes to the code */
    int num_line = STARTING_LINE; /* counts the line number, used for error handlaing purpose */

    /* go throught all of the lines of code (of assembly file ), and put them inside code nodes
    (each code node contains a line of text/code from the assembly file)*/
    while(getLine(buffer, error, fptr, num_line, is_print)) {
        if (*error == ERROR_MAXED_OUT_LINE_LENGTH) {
            num_line++;
            handleError(error, num_line, is_print);
            *error = NO_ERROR;
            continue;
        }
        /*Create a new code node*/
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
void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, int* pnum_tokens, char** tokens, bool* is_print, Error* error) {
    MacroNode* new_macro_node;      /* Stores a new macro node */
    MacroNode* temp_macro_node;     /* Temporarily holds a macro node for iteration */
    MacroNode* test_macro_node;     /* Used to test for duplicated macro definitions */
    CodeNode* new_code_node;        /* Stores a new code node */
    CodeNode* new_code_node_head;   /* Stores the head of the new code nodes linked list */
    CodeNode* curr_code_node;       /* Iterates through the code nodes */
    int num_line = STARTING_LINE;   /* Keeps track of the line number in the source file */

    temp_macro_node = NULL;
    curr_code_node = *code_node;
    /* goes throught all of the code lines */
    while (curr_code_node) {
        tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, is_print, error);
        if (*error == ERROR_MEMORY_ALLOCATION) return;

        /* if there is a mcro declaration line */
        if (*pnum_tokens == 2 && !strcmp(tokens[FIRST_WORD], "mcro") ) {

            /* check if name for macro is illegal */
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

            /* allocating memory for macro node and inserting the macro name into it */
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

            /* while there are still code lines (of assembly), and the line isn't endmcro */
            while(curr_code_node && strcmp(tokens[FIRST_WORD], "endmcro")) {

                /* allocating memory for code node (text line of code) */
                if (new_code_node->code_row) {
                    new_code_node->next = (CodeNode*) allocateMemory(sizeof(CodeNode), is_print, error);
                    if (*error == ERROR_MEMORY_ALLOCATION) return;
                    new_code_node = new_code_node->next;
                }

                /* saving the code line text inside the code node */
                new_code_node->code_row = my_strdup(curr_code_node->code_row, is_print, error);
                if (*error == ERROR_MEMORY_ALLOCATION) return;

                /* going to the next line of code (of assembler file )*/
                curr_code_node = curr_code_node->next;
                num_line++;
                tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens, is_print, error);
                if (*error == ERROR_MEMORY_ALLOCATION) return;
            }

            /* creating linked list from macro nodes - adding new nodes to existing linked list or creating the head of linked list */
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
        /* go to next line of code (of assembly file ) */
        curr_code_node = curr_code_node->next;
        num_line++;
    }
}

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
void insertMacrosToCode(CodeNode **code, MacroNode **macros, char *tokens[], int *pnum_tokens, bool* is_print, Error* error) {
    /* Declare variables */
    MacroNode *current_macro;            /* Current macro node being checked */
    CodeNode *current_code;              /* Current code node being processed */
    CodeNode *current_macro_code;        /* Current code node of a macro being inserted */
    CodeNode *prev_code;                 /* Previous code node */
    CodeNode *temp;                      /* Temporary code node pointer */
    CodeNode* temp_to_delete;            /* Temporary code node pointer for deletion */
    CodeNode *first_macro_code_node;     /* First code node of a macro being inserted */
    CodeNode *latest_macro_code_node = NULL; /* Latest code node of a macro being inserted */

    bool is_first_mcro_line = true;      /* Flag to track the first line of a macro */
    bool macro_found = false;            /* Flag to track if a macro is found */
    bool first_connection = true;        /* Flag to track the first connection of macro code nodes */
    bool prev_code_changed = false;      /* Flag to track if the previous code node changed */
    bool code_not_points_to_start = true; /* Flag to track if the code pointer doesn't point to the start */

    /* Initialize variables */
    current_code = *code;
    prev_code = (CodeNode *)allocateMemory(sizeof(CodeNode), is_print, error);
    prev_code->code_row = my_strdup(" ", is_print, error);
    temp_to_delete = prev_code;
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
                /* if found a mcro name */
                if (!strcmp(current_macro->macro_name, tokens[FIRST_WORD])) {
                    first_connection = true;
                    macro_found = true;

                    /* go throught all of the mcro code node, code lines 
                       and copy it to another code node
                    */
                    current_macro_code = current_macro->code_node;
                    while (current_macro_code)
                    {
                        /* copy the first code line from macro */
                        if (is_first_mcro_line)
                        {
                            first_macro_code_node = (CodeNode *)allocateMemory(sizeof(CodeNode), is_print, error);
                            if (*error == ERROR_MEMORY_ALLOCATION) return;
                            first_macro_code_node->code_row = my_strdup(current_macro_code->code_row, is_print, error);
                            if (*error == ERROR_MEMORY_ALLOCATION) return;

                            is_first_mcro_line = false;
                            current_macro_code = current_macro_code->next;
                        }
                        else /* copy the rest */
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
        /* go to the next line and save the latest line in prev_code */
        prev_code = current_code;
        prev_code_changed = true;
        current_code = current_code->next;
        if (code_not_points_to_start && prev_code_changed)
        {
            *code = prev_code;
            code_not_points_to_start = false;
        }
        
    }
    if (prev_code_changed)
    {
        free(temp_to_delete->code_row);
        free(temp_to_delete);
    }
}
