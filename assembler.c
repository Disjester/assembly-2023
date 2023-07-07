#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void firstIteration(short* memory, CodeNode* code, LabelNode* labels, Error* error) {
    CodeNode* temp_code;
    bool label_flag = false;
    int i;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int DC, IC;
    int num_tokens = 0;
    int token_counter = 0;
    int memory_counter = 100;
    short data[];

    DC = IC = 0;
    temp_code = code;
    while(temp_code) {
        tokenizeInput(temp_code->code_row, tokens, &num_tokens);
        if(isLabel(tokens[token_counter])) {
            printf("I SEE LABEL HERE: %s\n",temp_code->code_row);
            label_flag = true;
            token_counter++;
        }
        if(isData(tokens[token_counter])) {
            if (label_flag) {
                /*saveLabel(labels, memory, &memory_counter);*/ /*To be defined*/
            }
            /*setData()*/
            for (i = 0; i < i; i++) {
                pushToMemory(&memory_counter, memory, data[i]);
            }
            token_counter++;
            printf("I SEE DATA  HERE: %s\n", temp_code->code_row);
        }
        if(isExternOrEntry(tokens[token_counter])) {
            
        }
        token_counter = 0;
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

int isData(char* word){
    if (!strcmp(word, ".data"))
    {
        return 1;
    }
    

    return (!strcmp(word, ".string")) ? 2:false;
}

void pushToMemory(int* memory_counter, short* memory, short memoryField) {
    memory[(*memory_counter)++] = memoryField;
}