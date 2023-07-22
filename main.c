#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

int main (int argc, char** argv) {
    CodeNode* code, *test_code;
    Error error = NO_ERROR;
    LabelNode* labels = NULL;
    short memory[1024];
    int DC, IC;

    code = preproccessor(argv[1], &error);
    test_code = code;
    while (test_code) {
        printf("%s\n", test_code->code_row);
        test_code = test_code->next;
    }
    if (error != NO_ERROR) {
        return 0;
    }
    firstIteration(memory, code, &labels, &DC, &IC, &error);
    if (error != NO_ERROR) {
        return 0;
    }
    secondIteration(memory, code, labels, &DC, &IC, &error, argv[1]);
    if (error != NO_ERROR) {
        return 0;
    }

    return 1;
}