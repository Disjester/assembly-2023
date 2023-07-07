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
    short data[100];

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
            if (checkDataLine(tokens, num_tokens, label_flag))
            {
                printf("correct line\n");
            }
            else
            {
                printf("incorrect data line\n");
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


short isData(char* word){
    if (!strcmp(word, ".data"))
    {
        return 1;
    }
    

    return (!strcmp(word, ".string")) ? 2:false;
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
    if (string[i] != '"')
    {
        return false;
    }
    i++;
    bool quote = false;
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
    int token_index = 0;
    
    if (num_tokens < (2 + label))
    {
        printf("The line is missing arguments\n");
        return false;
    }
    
    if (isData(tokens[0 + label]) == STRING)
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
    
    if (isData(tokens[0 + label]) == DAT)
    {
        if (num_tokens % 2 == (0 + label))
        {
            printf("wrong number of ',' \n");
            return false;
        }

        for (; token_index + label < num_tokens; token_index++)
        {
            if (!isNumber(tokens[token_index]))
            {
                printf("this: %s is not a number\n",tokens[token_index]);
                return false;
            }
            
        }
    }

    return false;
}

void pushToMemory(int* memory_counter, short* memory, short memoryField) {
    memory[(*memory_counter)++] = memoryField;
}