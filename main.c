#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"
 
int main (int argc, char** argv) {
    CodeNode* code; /*Head of the code*/
    Error error;    /*Error variable for error handling*/
    LabelNode* labels; /*Head to linked list of labels*/
    LabelNode* externals; /*Head to linked list of externals*/
    short memory[MAX_MEMORY_SIZE]; /*Array that represents memory*/
    int memory_idx; /*Index to the last element in the memory*/
    int DC, IC; /*Data counter and Instructions counter*/
    int i; /*Index*/
    bool is_print; /*Flag if the program will create output files or not*/
    
    /* prints an error message, if no argument have been provided to the programm */
    
    if (argc <= 1) {
        error = ERROR_FILE_HANDLE;
        handleError(&error, 0, &is_print);
        return 0;
    }
    
   /* Iterate through the command line arguments */
    for (i = 1; i < argc; i++) {
        error = NO_ERROR;
        labels = NULL;
        memory_idx = DEFAULT_MEMORY_INDEX;
        DC = IC = 0;
        is_print = true;
        externals = NULL;

        /* Preprocess the source file */
        code = preproccessor(argv[i], &is_print, &error);
        if (error != NO_ERROR) continue;

        createCodeFileWithoutMacros(argv[i], code, &is_print, &error);
        if (error != NO_ERROR) continue;

        /* Perform the first iteration of assembly */
        firstIteration(memory, &memory_idx, code, &labels, &DC, &IC, &is_print, &error);

        /* Perform the second iteration of assembly */
        secondIteration(memory, &memory_idx, code, labels, &DC, &IC, &error, argv[i], externals, &is_print);
        if (error != NO_ERROR) continue;
        
    }

    /* Return success status */
    return 1;
}