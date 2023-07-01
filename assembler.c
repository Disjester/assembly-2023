#include "libs.h"
#include <string.h>

void firstIteration(short* memory, CodeNode* code, LabelNode* labels) {
    CodeNode* temp_code;
    char* label_flag;


    temp_code = code;

    while(temp_code) {
        if(strstr(temp_code->code_row, ":")) {
            
        }



        temp_code = temp_code->next;
    }
}