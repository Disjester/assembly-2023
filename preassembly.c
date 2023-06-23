#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int* pnum_tokens);
void freeLinkedList(CodeNode* head);
int getLine(char* line, Error* error, FILE* file);
void cleanLine(char* line);
void scanCodeForMacroDefinitions(CodeNode** code_node, MacroNode** macro_node, Error* error, int* pnum_tokens, char** tokens);
void preproccessor(char* file_name);


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

    while (macros) {
        printf("Macro name: %s\nCode: \n", macros->macro_name);
        while (macros->code_node) {
            printf("%s\n", macros->code_node->code_row);
            macros->code_node = macros->code_node->next;
        }
        macros = macros->next;
        printf("\n");
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
    CodeNode* new_code_node;
    CodeNode* new_code_node2;
    CodeNode* new_code_node_head;
    CodeNode* temp;

    while (*code_node) {
        tokenizeInput((*code_node)->code_row, tokens, pnum_tokens);
        if (*pnum_tokens == 2 && !strcmp(tokens[0], "mcro") ) {
            if (*macro_node) {
                while ((*macro_node)->next) {
                    *macro_node = (*macro_node)->next;
                }
                new_macro_node = (MacroNode*) malloc(sizeof(MacroNode));
                /*TBD*/
            } else {
                new_macro_node = (MacroNode*) malloc(sizeof(MacroNode));
                new_macro_node->next = NULL;
                new_macro_node->code_node = (CodeNode*) malloc(sizeof(CodeNode));
                new_macro_node->macro_name = (char*) malloc(sizeof(char)*(strlen(tokens[1])));

                strcpy(new_macro_node->macro_name, tokens[1]);
                temp = (*code_node)->next;
                tokenizeInput(temp->code_row, tokens, pnum_tokens);

                new_code_node = (CodeNode*) malloc(sizeof(CodeNode));
                new_code_node->code_row = NULL;
                new_code_node_head = new_code_node;

                while(temp && strcmp(tokens[0], "endmcro")) {
                    if (new_code_node->code_row) {
                        new_code_node2 = (CodeNode*) malloc(sizeof(CodeNode));
                        new_code_node->next = new_code_node2;
                        new_code_node = new_code_node->next;
                    }
                    new_code_node->code_row = (char*) malloc(sizeof(char)*(strlen(temp->code_row)));
                    strcpy(new_code_node->code_row, temp->code_row);


                    temp = temp->next;
                    tokenizeInput(temp->code_row, tokens, pnum_tokens);
                }
                new_macro_node->code_node = new_code_node_head;
                *macro_node = new_macro_node;
            }
        }
        *code_node = (*code_node)->next;
    }
}


void macrosToValues(CodeNode** code, MacroNode** macros, char *tokens[], int* pnum_tokens) {
    MacroNode* current_macro;
    CodeNode* current_code;
    CodeNode* current_macro_code;

    current_code = *code;

    while (current_code) {
        tokenizeInput(current_code->code_row, tokens, pnum_tokens);

        if (*pnum_tokens == 1) {
            current_macro = *macros;

            while (current_macro) {
                if (!strcmp(current_macro->macro_name, tokens[0])) {
                    /* Replace the macro name with the code lines */
                    current_macro_code = current_macro->code_node;

                    while (current_macro_code) {
                        printf("%s\n", current_macro_code->code_row); /* Print the code line */
                        current_macro_code = current_macro_code->next;
                    }

                    break; /* Exit the loop if the macro is found */
                }

                current_macro = current_macro->next;
            }
        }

        current_code = current_code->next;
    }
}
