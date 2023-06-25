#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

int main (int argc, char** argv) {
    preproccessor(argv[1]); /*HAS TO BE CHECKED!*/
    
    return 1;
}

void preproccessor(char* file_name) {
    CodeNode* code;
    Error* error = NO_ERROR;
    MacroNode* macros;

    FILE* file;

    char** tokens;

    int num_tokens = 0;

    tokens = malloc(MAX_TOKENS * sizeof(char *));
    file = fopen(file_name, "r");

    if (file == NULL) {
        perror("Error opening file\n");
    }

    code = createLinkedListFromFile(file, error, tokens, &num_tokens);
    scanCodeForMacroDefinitions(&code, &macros, error, &num_tokens, tokens);


    macrosToValues(&code, &macros, tokens, &num_tokens);

    while (macros) {
        printf("\nMacro name: %s\nCode: \n", macros->macro_name);
        while (macros->code_node) {
            printf("%s\n", macros->code_node->code_row);
            macros->code_node = macros->code_node->next;
        }
        macros = macros->next;
    }

    printf("\nFINAL CODE:\n\n");
    while (code) {
        printf("%s\n", code->code_row);
        code = code->next;
    }
}

/**
 * @brief Create a Linked List From File object
 * 
 * @param file 
 * @return CodeNode* 
 */
CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int* pnum_tokens) {
    char buffer[MAX_LINE_LENGTH];
    CodeNode *head = NULL, *temp = NULL, *node = NULL;

    while(getLine(buffer, error, file)) {
        /*Create a new node*/
        node = (CodeNode*)malloc(sizeof(CodeNode));
        if(!node) {
            printf("Error allocating memory for new node.\n");
            return NULL;
        }

        /*printing the contents of the buffer, to see what's inside*/
        node->code_row = (char*) malloc(strlen(buffer) + 1);

        /* Copy the string from buffer to the new node*/
        strcpy(node->code_row, buffer);
        node->next = NULL;
        
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

/**
 * @brief frees all the memmory allocated to the linked list ( code rows)
 * 
 * @param head 
 */
void freeLinkedList(CodeNode* head) {
    CodeNode* tmp;

    while (!head) {
        tmp = head;
        head = head->next;
        free(tmp);
    }
}

int getLine(char* line, Error* error, FILE* file) {
    char x; /*current symbol in the input stream*/
    int i = 0;
    cleanLine(line);
    while ((x = fgetc(file)) != '\n' && x != EOF) {
        if (i == MAX_LINE_LENGTH) {
            *error = ERROR_MAXED_OUT_LINE_LENGTH;
            /*skipping to the next line*/
            while ((x = fgetc(file)) != '\n' && x != EOF) {
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

        if (i != 0 && (x == ',' || x == ':')) {
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
        line[0] = '\n';
        return 1;
    }
    return i;
}

void cleanLine(char* line) {
    int i;
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        line[i] = '\0';
    }
}

void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, Error* error, int* pnum_tokens, char** tokens) {
    MacroNode* new_macro_node;
    MacroNode* temp_macro_node;
    CodeNode* new_code_node;
    CodeNode* new_code_node2;
    CodeNode* new_code_node_head;
    CodeNode* curr_code_node;

    temp_macro_node = *macro_node;
    curr_code_node = *code_node;
    while (curr_code_node) {
        tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens);
        if (*pnum_tokens == 2 && !strcmp(tokens[0], "mcro") ) {
            if (temp_macro_node) {
                while (temp_macro_node->next) {
                    temp_macro_node = temp_macro_node->next;
                }
                new_macro_node = (MacroNode*) malloc(sizeof(MacroNode));
                new_macro_node->next = NULL;
                new_macro_node->code_node = (CodeNode*) malloc(sizeof(CodeNode));
                new_macro_node->macro_name = (char*) malloc(sizeof(char)*(strlen(tokens[1])));

                strcpy(new_macro_node->macro_name, tokens[1]);
                curr_code_node = curr_code_node->next;
                tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens);

                new_code_node = (CodeNode*) malloc(sizeof(CodeNode));
                new_code_node->code_row = NULL;
                new_code_node_head = new_code_node;

                while(curr_code_node && strcmp(tokens[0], "endmcro")) {
                    if (new_code_node->code_row) {
                        new_code_node2 = (CodeNode*) malloc(sizeof(CodeNode));
                        new_code_node2->next = NULL;
                        new_code_node->next = new_code_node2;
                        new_code_node = new_code_node->next;
                    }
                    new_code_node->code_row = (char*) malloc(sizeof(char)*(strlen(curr_code_node->code_row)));
                    strcpy(new_code_node->code_row, curr_code_node->code_row);

                    curr_code_node = curr_code_node->next;
                    tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens);
                }
                new_macro_node->code_node = new_code_node_head;
                temp_macro_node->next = new_macro_node;
                temp_macro_node = temp_macro_node->next;
            } else {
                new_macro_node = (MacroNode*) malloc(sizeof(MacroNode));
                new_macro_node->next = NULL;
                new_macro_node->code_node = (CodeNode*) malloc(sizeof(CodeNode));
                new_macro_node->macro_name = (char*) malloc(sizeof(char)*(strlen(tokens[1])));

                strcpy(new_macro_node->macro_name, tokens[1]);
                curr_code_node = curr_code_node->next;
                tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens);

                new_code_node = (CodeNode*) malloc(sizeof(CodeNode));
                new_code_node->code_row = NULL;
                new_code_node_head = new_code_node;

                while(curr_code_node && strcmp(tokens[0], "endmcro")) {
                    if (new_code_node->code_row) {
                        new_code_node2 = (CodeNode*) malloc(sizeof(CodeNode));
                        new_code_node2->next = NULL;
                        new_code_node->next = new_code_node2;
                        new_code_node = new_code_node->next;
                    }
                    new_code_node->code_row = (char*) malloc(sizeof(char)*(strlen(curr_code_node->code_row)));
                    strcpy(new_code_node->code_row, curr_code_node->code_row);

                    curr_code_node = curr_code_node->next;
                    tokenizeInput(curr_code_node->code_row, tokens, pnum_tokens);
                }
                new_macro_node->code_node = new_code_node_head;
                *macro_node = new_macro_node;
                temp_macro_node = *macro_node;
            }
        }
        curr_code_node = curr_code_node->next;
    }
}

void macrosToValues(CodeNode **code, MacroNode **macros, char *tokens[], int *pnum_tokens)
{
    MacroNode *current_macro;
    CodeNode *current_code;
    CodeNode *current_macro_code;
    CodeNode *prev_code; /* Pointer to the previous code node*/

    current_code = *code;
    prev_code = NULL;

    while (current_code)
    {
        tokenizeInput(current_code->code_row, tokens, pnum_tokens);

        /*if its 1 word*/
        if (*pnum_tokens == 1)
        {
            current_macro = *macros;

            /*going throught all the macros*/
            while (current_macro)
            {
                /*if the word is a macro name*/
                if (!strcmp(current_macro->macro_name, tokens[0]))
                {
                    /* Replace the macro name with the code lines */
                    current_macro_code = current_macro->code_node;

                    /* Traverse the macro code lines and insert them into the linked list*/
                    while (current_macro_code)
                    {
                        /* Create a new code node*/
                        CodeNode *new_code_node = (CodeNode *)malloc(sizeof(CodeNode));
                        new_code_node->code_row = strdup(current_macro_code->code_row);
                        new_code_node->next = NULL;

                        /* Update the next pointer of the previous code node or the head pointer*/
                        if (prev_code)
                            prev_code->next = new_code_node;
                        else
                            *code = new_code_node;

                        /* Update the previous code node to the current one*/
                        prev_code = new_code_node;

                        current_macro_code = current_macro_code->next;
                    }

                    /* Break out of the loop if the macro is found*/
                    break;
                }

                current_macro = current_macro->next;
            }
        }

        prev_code = current_code;
        current_code = current_code->next;
    }
}
