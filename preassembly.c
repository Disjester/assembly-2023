#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libs.h"

CodeNode* createLinkedListFromFile(FILE* file, Error* error, char *tokens[], int* pnum_tokens);
void freeLinkedList(CodeNode* head);
int getLine(char* line, Error* error, FILE* file);
void cleanLine(char* line);
void scanCodeForMacroDefinitions(CodeNode* code_node, MacroNode* macro_node, Error* error);
void preproccessor(char* file_name);


int main (int argc, char** argv) {
    preproccessor(argv[1]); /*HAS TO BE CHECKED!*/
    
    return 1;
}

void preproccessor(char* file_name) {
    CodeNode* cn;
    Error* error = NO_ERROR;
    /*MacroNode* mn;*/
    FILE* file;

    char** tokens;
    int num_tokens = 0;

    file = fopen(file_name, "r");

    if (file == NULL) {
        perror("Error opening file\n");
    }

    cn = createLinkedListFromFile(file, error, tokens, &num_tokens);
    while(cn) {
        printf("%s\n", cn->code_row);
        cn = cn->next;
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

    while(getLine(buffer, error, file) != 0) {
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

        /*tokenizeInput(node->code_row, tokens, pnum_tokens);
        if (!strcmp(tokens[0], "MAIN:"))
        {
            printf("correct\n");
        }*/

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

    while (head != NULL) {
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

void scanCodeForMacroDefinitions(CodeNode* code_node, MacroNode* macro_node, Error* error) {
    /*MacroNode* new_macro_node;
    tokenizeInput(...);
    while (code_node) {
        if (num_tokens == 2 && !strcmp(tokens[0], "mcro") ) {
            if (macro_node) {
                while (macro_node->next) {
                    macro_node = macro_node->next;
                }
                new_macro_node = (MacroNode*) malloc(sizeof(MacroNode));

            } else {
                
            }
        }
        code_node = code_node->next;
    }
    */
}


void macrosToValues(CodeNode* code, MacroNode* macros, char *tokens[], int* pnum_tokens){
    return;
}