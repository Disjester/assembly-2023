#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "libs.h"

static const Command commands[MAX_COMMAND_LENGTH] = {
    {"mov",  0x0, 2, {1, 1, 1}, {0, 1, 1}},
    {"cmp",  0x1, 2, {1, 1, 1}, {1, 1, 1}},
    {"add",  0x2, 2, {1, 1, 1}, {0, 1, 1}},
    {"sub",  0x3, 2, {1, 1, 1}, {0, 1, 1}},
    {"not",  0x4, 1, {0, 0, 0}, {0, 1, 1}},
    {"clr",  0x5, 1, {0, 0, 0}, {0, 1, 1}},
    {"lea",  0x6, 2, {0, 1, 0}, {0, 1, 1}},
    {"inc",  0x7, 1, {0, 0, 0}, {0, 1, 1}},
    {"dec",  0x8, 1, {0, 0, 0}, {0, 1, 1}},
    {"jmp",  0x9, 1, {0, 0, 0}, {0, 1, 1}},
    {"bne",  0xA, 1, {0, 0, 0}, {0, 1, 1}},
    {"red",  0xB, 1, {0, 0, 0}, {0, 1, 1}},
    {"prn",  0xC, 1, {0, 0, 0}, {1, 1, 1}},
    {"jsr",  0xD, 1, {0, 0, 0}, {0, 1, 1}},
    {"rts",  0xE, 0, {0, 0, 0}, {0, 0, 0}},
    {"stop", 0xF, 0, {0, 0, 0}, {0, 0, 0}}
};


void firstIteration(short* memory, CodeNode* code, LabelNode* labels, Error* error) {
    CodeNode* temp_code;
    LabelNode* test_label_node;
    bool label_flag = false;
    int i;
    int def_extern_mem = -1;
    int place = 0;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int DC, IC;
    int num_tokens = 0;
    int token_idx = 0;
    int memory_idx = 100;
    short data[100];

    DC = IC = 0;
    cleanMemory(memory);
    temp_code = code;
    while(temp_code) {
        tokenizeInput(temp_code->code_row, tokens, &num_tokens);
        if(isLabel(tokens[token_idx], true)) {
            printf("I  SEE  LABEL  HERE: %s\n",temp_code->code_row);
            label_flag = true;
            token_idx++;
        }
        switch (isDotType(tokens[token_idx])) {
            case DOT_DATA:
                if (label_flag) {
                    insertNewLabel(&labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, &DC);
                    test_label_node = labels;
                    printf("CURRENT LABEL TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag)) {
                    printf("I  SEE   DATA  HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = token_idx; i < num_tokens; i += 2) {
                        pushToMemory(&memory_idx, memory, atoi(tokens[i]));
                        DC++;
                    }
                }
                printf("CURRENT      MEMORY: ");
                for (i = 100; i < memory_idx; i++) {
                    printf("%d:%d ", i, memory[i]);
                }
                printf("\n");
                break;
            case DOT_STRING:
                if (label_flag) {
                    insertNewLabel(&labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, &DC);
                    test_label_node = labels;
                    printf("CURRENT LABEL TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag)) {
                    printf("I  SEE STRING  HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = 1; i < (strlen(tokens[token_idx])-1); i++) {
                        pushToMemory(&memory_idx, memory, tokens[token_idx][i]);
                        DC++;
                    }
                    pushToMemory(&memory_idx, memory, '\0');
                    DC++;
                }
                printf("CURRENT      MEMORY: ");
                for (i = 100; i < memory_idx; i++) {
                    printf("%d:%d ", memory[i], i);
                }
                printf("\n");
                break;
            case DOT_EXTERN:
                place = 1;
                for (; place < num_tokens; place++) {
                    if (isLabel(tokens[place], false)) {
                        insertNewLabel(&labels, tokens[place], LABEL_TYPE_EXTERNAL, &def_extern_mem);
                        printf("I SEE EXTERNAL HERE: %s\n", temp_code->code_row);
                    }
                    else {
                        break;
                    }
                    
                }
                test_label_node = labels;
                printf("CURRENT LABEL TABLE: ");
                while (test_label_node) {
                    printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                    test_label_node = test_label_node->next;
                }
                printf("\n");
                break;
            case DOT_ENTRY:
                break;
            case DOT_OTHER:
                if (label_flag) {
                    insertNewLabel(&labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_CODE, &IC);
                    test_label_node = labels;
                    printf("CURRENT LABEL TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (label_flag) {
                    checkCommand(tokens[1]);
                }
                else {
                    checkCommand(tokens[0]);
                }
                
                break;
        }

        printf("CURRENT  IC  AND DC: %d, %d\n", IC, DC);

        token_idx = 0;
        label_flag = false;
        temp_code = temp_code->next;
    }
}


bool isLabel(char* word, bool colon){
    bool flag = false;
    int i = 0;
    if (!isalpha(word[i++]))
    {
        return flag;
    }
    
    for (; word[i] != '\0'; i++)
    {
        if (colon == true && word[i] == ':' && word[i+1] == '\0')
        {
            flag = true;
            return flag;
        }
        
        if (!isalpha(word[i]) && !isdigit(word[i]))
        {
            return flag;
        }
        
    }
    if (colon)
    {
        flag = false;
    }
    else
    {
        flag = true;
    }
    
    return flag;
}


short isDotType(char* word){
    if (!strcmp(word, ".data"))
    {
        return DOT_DATA;
    }
    if (!strcmp(word, ".string"))
    {
        return DOT_STRING;
    }
    if (!strcmp(word, ".entry"))
    {
        return DOT_ENTRY;
    }
    
    return (!strcmp(word, ".extern")) ? DOT_EXTERN:DOT_OTHER;
}


LabelType getLabelType(char* label, LabelNode* LabelNode){
    if (LabelNode != NULL)
    {
        do
        {
            if (!strcmp(label, LabelNode->label_name))
            {
                printf("found Label %s is of type: %d\n", label, LabelNode->label_type);
                return LabelNode->label_type;
            }
            LabelNode = LabelNode->next;
        }
        while (LabelNode->next != NULL);
    }
    
    printf("error, haven't found the label\n");
    return 0;

}

bool isString(char* string){
    int i = 0;
    bool quote = false;
    if (string[i++] != '"')
    {
        return false;
    }
    for ( ; i < strlen(string); i++)
    {
        if (quote)
        {
            return false;
        }
        
        if (string[i] == '"')
        { 
            quote = true;
        }
        
    }

    return quote;
}

bool isNumber(char* word){
    int i = 0;
    int len = strlen(word);
    
    
    for ( ; i < len; i++)
    {
        if (!isdigit(word[i]))
        {
            printf("not a number: %c\n", word[i]);
            return false;
        }
        
    }
    return true;
    
}

bool checkDataLine(char** tokens, int num_tokens, bool label){
    int token_index = 1;
    if (num_tokens < (2 + label))
    {
        printf("The line is missing arguments\n");
        return false;
    }
    
    if (isDotType(tokens[0 + label]) == DOT_STRING)
    {
        if (num_tokens > (2 + label))
        {
            printf("too many arguments\n");
            
            return false;
        }
        if (isString(tokens[1 + label]))
        {
            return true;
        }
        
    }
    
    if (isDotType(tokens[0 + label]) == DOT_DATA)
    {
        if (num_tokens % 2 == (1 + label))
        {
            printf("wrong number of ',' \n");
            return false;
        }

        token_index += label;

        for (; token_index< num_tokens; token_index+=2)
        {
            if (!isNumber(tokens[token_index]))
            {
                return false;
            }

        }
        for (; token_index + 1 < num_tokens; token_index += 2)
        {
            if (strcmp(tokens[token_index], ","))
            {
                printf("Missing a ,");
                return false;
            }
        }
        return true;
    }

    return false;
}

void pushToMemory(int* memory_idx, short* memory, short memoryField) {
    memory[(*memory_idx)++] = memoryField;
}

void cleanMemory(short* memory) {
    int i;

    for (i = 0; i <= MAX_MEMORY_SIZE; i++) {
        memory[i] = -1;
    }
}

void insertNewLabel(LabelNode** labels, char* label_name, LabelType label_type, int* memory_idx) {
    LabelNode* temp_label;
    LabelNode* new_label;
    temp_label = *labels;
    if (temp_label) {
        while(temp_label && temp_label->next) {
            temp_label = temp_label->next;
        }
        new_label = (LabelNode*) malloc(sizeof(LabelNode));
        new_label->label_name = label_name;
        new_label->label_type = label_type;
        new_label->memory_adress = *memory_idx;
        new_label->next = NULL;
        temp_label->next = new_label;
    } else {
        *labels = (LabelNode*) malloc(sizeof(LabelNode));
        (*labels)->label_name = label_name;
        (*labels)->label_type = label_type;
        (*labels)->memory_adress = *memory_idx;
        (*labels)->next = NULL;
    }
}

char* removeColon(char* str) {
    str[strlen(str)-1] = '\0';
    return str;
}

short checkCommand(char* word){
    int i = 0;
    for (; i < NUM_OF_COMMANDS; i++)
    {
        if (!strcmp((char*)commands[i].command, word))
        {
            printf("I SEE COMMAND  HERE: %s! its opcode is: %d\n", (char*)commands[i].command, commands[i].opcode);
            return commands[i].opcode;
        }
        
    }
    printf("Error, command unrecognized command: %s\n", word);
    return -1;
}