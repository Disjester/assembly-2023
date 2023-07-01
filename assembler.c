#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void firstIteration(short* memory, CodeNode* code_node) {
    
}

bool isLabel(char* word){
    bool flag = false;
    int i = 0;
    if (!isalpha(word[i]))
    {
        return flag;
    }
    
    for (; word[i] != '\0'; i++)
    {
        if (!isalpha(word[i]) && !isdigit(word[i]))
        {
            return flag;
        }
        
    }
    if (word[i-1] == ':')
    {
        flag = true;
    }

    return flag;
}