#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

int main (int argc, char** argv) {
    CodeNode* code;
    Error* error = NO_ERROR;
    LabelNode* labels = NULL;
    short memory[1024];
    int DC, IC;

    code = preproccessor(argv[1]); /*HAS TO BE CHECKED!*/
    
    firstIteration(memory, code, labels, &DC, &IC, error);
    secondIteration(memory, code, labels, &DC, &IC, error);
    return 1;
}