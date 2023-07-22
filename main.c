#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

int main (int argc, char** argv) {
    CodeNode* code;
    Error error = NO_ERROR;
    LabelNode* labels = NULL;
    short memory[1024];
    int DC, IC;

    code = preproccessor(argv[1], &error);
    while (code) {
        printf("%s\n", code->code_row);
        code = code->next;
    }
    if (error != NO_ERROR) {
        return 0;
    }
    firstIteration(memory, code, &labels, &DC, &IC, &error);
    secondIteration(memory, code, labels, &DC, &IC, &error, argv[1]);

    return 1;
}