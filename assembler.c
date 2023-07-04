#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

void firstIteration(short* memory, CodeNode* code, LabelNode* labels, Error* error) {
    CodeNode* temp_code;
    bool label_flag = false;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int DC, IC;
    int num_tokens = 0;
    int token_counter = 0;
    int memory_counter = 100;

    DC = IC = 0;
    temp_code = code;
    while(temp_code) {
        tokenizeInput(temp_code->code_row, tokens, &num_tokens);
        if(isLabel(tokens[token_counter])) {
            printf("I SEE LABEL HERE: %s\n",temp_code->code_row);
            label_flag = true;
            token_counter++;
        }
        if(isData(tokens[token_counter])) {
            if (label_flag) {
                /*saveLabel(labels, memory, &memory_counter);*/ /*To be defined*/
            }
            token_counter++;
            printf("I SEE DATA  HERE: %s\n", temp_code->code_row);
        }
        
        token_counter = 0;
        label_flag = false;
        temp_code = temp_code->next;
    }
}


bool isLabel(char* word){
    bool flag = false;
    int i = 0;
    if (!isalpha(word[i++]))
    {
        return flag;
    }
    
    for (; word[i] != '\0'; i++)
    {
        if (word[i] == ':' && word[i+1] == '\0')
        {
            flag = true;
            return flag;
        }
        
        if (!isalpha(word[i]) && !isdigit(word[i]))
        {
            return flag;
        }
        
    }

    return flag;
}

int isData(char* word){
    if (!strcmp(word, ".data"))
    {
        return 1;
    }
    

    return (!strcmp(word, ".string")) ? 2:false;
}

bool checkData(char* line, Error* error){
    int var = 0;
    char** tokens;
    int num_tokens = 0;
    tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    bool label = false;

    char* temp;
    tokenizeInput(line, tokens, &num_tokens);

    (isLabel(tokens[0])) ? true:false ; /*filler */
    /* Check if no arguments are provided after the operation */
    if (num_tokens == 1)
    {
        printf("Error: No arguments\n\n");
        return false;
    }


    /* Check for valid arguments if arguments are provided */
    if (num_tokens > 1)
    {
        if (isLabel(tokens[0]) && num_tokens == 2)
        {
            printf("Error: No arguments\n\n");
            return false;
        }
        
        /* If there's a comma directly after the operation name, it's an error */
        if (tokens[1][0] == ',')
        {
            printf("Error: illegal comma after command\n\n");
            return;
        }

        /* Check if the argument is a recognized complex variable */
        var = check_complex_variable(tokens[1], complex_var_names);
        if (var == -1)
        {
            printf("Error: Unrecognized Complex variable\n\n");
            return;
        }
        /* If only one argument is provided and it's valid, execute the operation */
        else if (num_tokens == 2)
        {
            func(&complex_vars[var]);
            return;
        }
    }

    /* If no arguments are provided, it's an error */
    if (num_tokens < 2)
    {
        printf("Error: Missing complex variable\n\n");
    }
 
    /* If there's extra text after the command, it's an error */
    printf("Error: Externous text at the end of command\n\n");
    return;
}