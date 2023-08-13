#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

int main (int argc, char** argv) {
    CodeNode* code;
    Error error;
    LabelNode* labels;
    LabelNode* externals;
    short memory[MAX_MEMORY_SIZE];
    int memory_idx;
    int DC, IC;
    int i;
    bool is_print;
    /*
    if (argc <= 1)
    {
        *error = ERROR_FILE_HANDLE;
        handleError(error, 0, &is_print);
        return 0;
    }
    */
    for (i = 1; i < argc; i++) {
        error = NO_ERROR;
        labels = NULL;
        memory_idx = MEMORY_INDEX;
        DC = IC = 0;
        is_print = true;
        externals = NULL;

        code = preproccessor(argv[i], &is_print, &error);
        if (error != NO_ERROR) continue;
        
        firstIteration(memory, &memory_idx, code, &labels, &DC, &IC, &is_print, &error);
        if (error != NO_ERROR || is_print == false) continue;

        /*secondIteration(memory, &memory_idx, code, labels, &DC, &IC, &error, argv[i], externals, &is_print);
        if (error != NO_ERROR) continue;*/
    }
    return 1;
}