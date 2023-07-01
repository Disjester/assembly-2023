#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void firstIteration(short* memory, CodeNode* code, LabelNode* labels) {
    CodeNode* temp_code;
    char* label_flag;


    temp_code = code;

    while(temp_code) {
        if(strstr(temp_code->code_row, ":")) {
            printf("%s  : %d\n",temp_code->code_row, isLabel(temp_code->code_row, strstr(temp_code->code_row, ":")));
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