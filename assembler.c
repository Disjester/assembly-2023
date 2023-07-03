#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void firstIteration(short* memory, CodeNode* code, LabelNode* labels) {
    CodeNode* temp_code;
    bool label_flag;
    char** tokens;
    int num_tokens = 0;


    temp_code = code;

    while(temp_code) {
        tokenizeInput(temp_code->code_row, tokens, &num_tokens);
        if(isLabel(token[0])) {
            printf("%s\n",temp_code->code_row);
        }




        temp_code = temp_code->next;
    }
}


bool isLabel(char* word, char* end){
    bool flag = false;
    int i = 0;
    if (!isalpha(word[i]))
    {
        return flag;
    }
    
    for (; &word[i] != end; i++)
    {
        if (!isalpha(word[i]) && !isdigit(word[i]))
        {
            return flag;
        }
        
    }
    flag = true;

    return flag;
}