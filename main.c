#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

int main (int argc, char** argv) {
    CodeNode* code;
    Error error = NO_ERROR;
    LabelNode* labels = NULL;
    short memory[MAX_MEMORY_SIZE];
    int memory_idx = 100;
    int DC, IC;
    int i;

    for (i = 1; i <= argc; i++) {
        error = NO_ERROR;
        labels = NULL;
        memory_idx = 100;
        DC = IC = 0;
        code = preproccessor(argv[1], &error);

        if (error != NO_ERROR) continue;
        
        firstIteration(memory, &memory_idx, code, &labels, &DC, &IC, &error);
        if (error != NO_ERROR) continue;

        secondIteration(memory, &memory_idx, code, labels, &DC, &IC, &error, argv[1]);
        if (error != NO_ERROR) continue;
    }
    return 1;
}