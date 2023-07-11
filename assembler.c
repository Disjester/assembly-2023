#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void firstIteration(short* memory, CodeNode* code, LabelNode* labels, Error* error) {
    CodeNode* temp_code;
    LabelNode* test_label_node;
    bool label_flag = false;
    int i;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int DC, IC = 0;
    int num_tokens = 0;
    int token_idx = 0;
    int memory_idx = 100;
    short data[100];

    cleanMemory(memory);
    temp_code = code;
    while(temp_code) {
        tokenizeInput(temp_code->code_row, tokens, &num_tokens);
        if(isLabel(tokens[token_idx])) {
            printf("I SEE LABEL HERE: %s\n",temp_code->code_row);
            label_flag = true;
            token_idx++;
        }
        switch (isDotType(tokens[token_idx])) {
            case DOT_DATA:
                if (label_flag) {
                    insertNewLabel(labels, tokens[token_idx-1], LABEL_TYPE_CODE, &memory_idx);
                    test_label_node = labels;
                    while (test_label_node) {
                        printf("CURRENT LABEL TABLE:   %s", test_label_node->label_name);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag)) {
                    printf("I SEE DATA  HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = token_idx; i < num_tokens; i += 2) {
                        pushToMemory(&memory_idx, memory, atoi(tokens[i]));
                    }
                }
                printf("CURRENT   MEMORY: ");
                for (i = 100; i < memory_idx; i++) {
                    printf("%d:%d ", i, memory[i]);
                }
                printf("\n");
                break;
            case DOT_STRING:
                if (label_flag) {
                    insertNewLabel(labels, tokens[token_idx-1], LABEL_TYPE_CODE, &memory_idx);
                    test_label_node = labels;
                    while (test_label_node) {
                        printf("CURRENT LABEL TABLE:   %s", test_label_node->label_name);
                        test_label_node = test_label_node->next;
                    }
                    getLabelType( test_label_node->label_name, test_label_node);
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag)) {
                    printf("I SEE DATA  HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = 1; i < (strlen(tokens[token_idx])-1); i++) {
                        pushToMemory(&memory_idx, memory, tokens[token_idx][i]);
                    }
                    pushToMemory(&memory_idx, memory, '\0');
                }
                printf("CURRENT   MEMORY: ");
                for (i = 100; i < memory_idx; i++) {
                    printf("%d:%d ", i, memory[i]);
                }
                printf("\n");
                break;
            case DOT_EXTERN:
                break;
            case DOT_ENTRY:
                break;
        }
        /*if(isExternOrEntry(tokens[token_counter])) {
            
        }*/
        token_idx = 0;
        label_flag = false;
        temp_code = temp_code->next;
    }
}


bool isLabel(char* word){
    bool flag = false;
    int i = 0;
    if (!isalpha(word[i++]))
    {
        return flag;
    }
    
    for (; word[i] != '\0'; i++)
    {
        if (word[i] == ':' && word[i+1] == '\0')
        {
            flag = true;
            return flag;
        }
        
        if (!isalpha(word[i]) && !isdigit(word[i]))
        {
            return flag;
        }
        
    }

    return flag;
}


short isDotType(char* word){
    if (!strcmp(word, ".data"))
    {
        return DOT_DATA;
    }
    if (!strcmp(word, ".string"))
    {
        return DOT_STRING;
    }
    if (!strcmp(word, ".entry"))
    {
        return DOT_ENTRY;
    }
    
    return (!strcmp(word, ".extern")) ? DOT_EXTERN:false;
}


LabelType getLabelType(char* label, LabelNode* LabelNode){
    while (LabelNode->next != NULL)
    {
        if (!strcmp(label, LabelNode->label_name))
        {
            printf("found Label %s is of type: %d\n", label, LabelNode->label_type);
            return LabelNode->label_type;
        }
        
    }
    printf("error, haven't found the label\n");
    return 0;
    /*Error - haven't found label in the LabelNodes*/
    
}
/*
bool checkData(char* line, Error* error){

    char** tokens;
    int num_tokens = 0;

    tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);

    tokenizeInput(line, tokens, &num_tokens);

    return (checkDataLine(tokens, *num_tokens, isLabel(tokens[0]))) ? true : false;
}
*/

bool isString(char* string){
    int i = 0;
    bool quote = false;
    if (string[i] != '"')
    {
        return false;
    }
    i++;
    for ( ; i < strlen(string); i++)
    {
        if (quote)
        {
            return false;
        }
        
        if (string[i] == '"')
        {
            quote = true;
        }
        
    }

    return quote;
}

bool isNumber(char* word){
    int i = 0;
    int len = strlen(word);
    
    
    for ( ; i < len; i++)
    {
        if (!isdigit(word[i]))
        {
            printf("not a number: %c\n", word[i]);
            return false;
        }
        
    }
    return true;
    
}

bool checkDataLine(char** tokens, int num_tokens, bool label){
    int token_index = 1;
    if (num_tokens < (2 + label))
    {
        printf("The line is missing arguments\n");
        return false;
    }
    
    if (isDotType(tokens[0 + label]) == DOT_STRING)
    {
        if (num_tokens > (2 + label))
        {
            printf("too many arguments\n");
            
            return false;
        }
        if (isString(tokens[1 + label]))
        {
            return true;
        }
        
    }
    
    if (isDotType(tokens[0 + label]) == DOT_DATA)
    {
        if (num_tokens % 2 == (1 + label))
        {
            printf("wrong number of ',' \n");
            return false;
        }

        token_index += label;

        for (; token_index< num_tokens; token_index+=2)
        {
            if (!isNumber(tokens[token_index]))
            {
                return false;
            }

        }
        for (; token_index + 1 < num_tokens; token_index+=2)
        {
            if (strcmp(tokens[token_index], ","))
            {
                printf("Missing a ,");
                return false;
            }
        }
        return true;
    }

    return false;
}

void pushToMemory(int* memory_idx, short* memory, short memoryField) {
    memory[(*memory_idx)++] = memoryField;
}

void cleanMemory(short* memory) {
    int i;

    for (i = 0; i <= MAX_MEMORY_SIZE; i++) {
        memory[i] = -1;
    }
}

void insertNewLabel(LabelNode* label, char* label_name, LabelType label_type, short* memory_idx) {
    LabelNode* temp_label;
    LabelNode* new_label;
    temp_label = label;
    if (temp_label) {
        while(temp_label->next) {
            temp_label = temp_label->next;
        }
        new_label = (LabelNode*) malloc(sizeof(LabelNode));
        new_label->label_name = label_name;
        new_label->label_type = label_type;
        new_label->next = NULL;
        temp_label->next = new_label;
    } else {
        label = (LabelNode*) malloc(sizeof(LabelNode));
        label->label_name = label_name;
        label->label_type = label_type;
        label->next = NULL;
    }
}