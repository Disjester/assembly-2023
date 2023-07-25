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


void firstIteration(short* memory, CodeNode* code, LabelNode** labels, int* DC, int* IC, Error* error) {
    CodeNode* temp_code;
    LabelNode* test_label_node;
    LabelNode* temp_label_node;
    bool label_flag = false;
    int i;
    int def_extern_mem = DEFAULT_EXTERN_MEMORY;
    int place;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int num_tokens = 0;
    int token_idx = 0;
    int memory_idx = 100;
    short data[100];
    int L = 0;
    
    if (*error == ERROR_MEMORY_ALLOCATION) return;
 
    printf("!!!   BEGGINING OF THE FIRST ITERATION   !!!\n\n");
    *DC = *IC = 0;
    cleanMemory(memory);
    temp_code = code;
    while(temp_code) {
        if (temp_code->code_row[0] == ';') {
            printf("I  SEE  COMMENT  HERE: %s\n", temp_code->code_row);
            temp_code = temp_code->next;
            continue;
        }
        if (temp_code->code_row[0] == '\n') {
            printf("I SEE EMPTY LINE HERE: %s\n", temp_code->code_row);
            temp_code = temp_code->next;
            continue;
        }
        tokenizeInput(temp_code->code_row, tokens, &num_tokens, error);
        if (*error == ERROR_MEMORY_ALLOCATION) return;

        if(isLabel(tokens[token_idx], true)) {
            printf("I  SEE   LABEL   HERE: %s\n",temp_code->code_row);
            label_flag = true;
            token_idx++;
        }
        switch (isDotType(tokens[token_idx])) {
            case DOT_DATA:
                if (label_flag) {
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, DC, error);
                    test_label_node = *labels;
                    printf("CURRENT  LABEL  TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag)) {
                    printf("I   SEE   DATA   HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = token_idx; i < num_tokens; i += 2) {
                        pushToMemory(&memory_idx, memory, atoi(tokens[i]));
                        (*DC)++;
                    }
                    printf("CURRENT   IC  AND  DC: %d, %d\n", *IC, *DC);
                }
                printf("CURRENT        MEMORY: ");
                for (i = 100; i < memory_idx; i++) {
                    printf("%d:%d ", i, memory[i]);
                }
                printf("\n");
                break;
            case DOT_STRING:
                if (label_flag) {
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, DC, error);
                    test_label_node = *labels;
                    printf("CURRENT  LABEL  TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag)) {
                    printf("I  SEE  STRING   HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = 1; i < (strlen(tokens[token_idx])-1); i++) {
                        pushToMemory(&memory_idx, memory, tokens[token_idx][i]);
                        (*DC)++;
                    }
                    pushToMemory(&memory_idx, memory, '\0');
                    (*DC)++;
                    printf("CURRENT  IC   AND  DC: %d, %d\n", *IC, *DC);
                }
                printf("CURRENT        MEMORY: ");
                for (i = 100; i < memory_idx; i++) {
                    printf("%d:%d ", memory[i], i);
                }
                printf("\n");
                break;
            case DOT_EXTERN:
                place = 1;
                for (; place < num_tokens; place++) {
                    if (isLabel(tokens[place], false)) {
                        insertNewLabel(labels, tokens[place], LABEL_TYPE_EXTERNAL, &def_extern_mem, error);
                        printf("I SEE  EXTERNAL  HERE: %s\n", temp_code->code_row);
                    }
                    else {
                        break;
                    }
                    
                }
                test_label_node = *labels;
                printf("CURRENT  LABEL  TABLE: ");
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
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_CODE, IC, error);
                    test_label_node = *labels;
                    printf("CURRENT  LABEL  TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkCommandLine(tokens, num_tokens, label_flag) != COMMAND_LINE_ERROR)
                {
                    L = checkCommandLine(tokens, num_tokens, label_flag);
                }
                
                *IC += L;
                L = 0;
                printf("THE IC IS : %d\n",*IC);
                break;
        }
        token_idx = 0;
        label_flag = false;
        temp_code = temp_code->next;
    }

    temp_label_node = *labels;
    while (temp_label_node) {
        if (temp_label_node->label_type == LABEL_TYPE_DATA) {
            temp_label_node->memory_adress += *IC;
        }
        temp_label_node = temp_label_node->next;
    }
 
    /**/
    test_label_node = *labels;
    printf("LABEL  TABLE  AFTER 1: ");
    while (test_label_node) {
        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
        test_label_node = test_label_node->next;
    }
    printf("\n\n");
}

void secondIteration(short* memory, CodeNode* code, LabelNode* labels, int* DC, int* IC, Error* error, char* file_name) {
    CodeNode* temp_code; 
    int token_idx = 0;
    bool label_flag = false;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int num_tokens = 0;
    int L = 0;

    printf("!!!   BEGGINING OF THE SECOND ITERATION   !!!\n\n");
    temp_code = code;
    *IC = 0;
    while (temp_code) {
        tokenizeInput(temp_code->code_row, tokens, &num_tokens, error);
        if(isLabel(tokens[token_idx], true)) {
            printf("I  SEE   LABEL   HERE: %s\n",temp_code->code_row);
            label_flag = true;
            token_idx++;
        }
        switch (isDotType(tokens[token_idx])) {
            case DOT_ENTRY:
                updateEntryLabels(labels, tokens, num_tokens, token_idx);
                break;
            case DOT_OTHER:
                if (checkCommandLine(tokens, num_tokens, label_flag) != COMMAND_LINE_ERROR)
                {
                    L = checkCommandLine(tokens, num_tokens, label_flag);
                }

                IC += L;
                break;
            case DOT_DATA:
            case DOT_EXTERN:
            case DOT_STRING:
                break;
        }
        temp_code = temp_code->next;
        token_idx = 0;
        label_flag = false;
        L = 0;
    }
    /*handleError(error);*/
    createOutputFiles(file_name, labels, error);
}

void updateEntryLabels(LabelNode* labels, char** tokens, int num_tokens, int token_idx) {
    LabelNode* temp_label;
    
    for (;token_idx < num_tokens; token_idx++) {
        if (isLabel(tokens[token_idx], false)) {
            temp_label = labels;
            while(temp_label) {
                if (!strcmp(temp_label->label_name, tokens[token_idx])) {
                    temp_label->label_type = LABEL_TYPE_ENTRY;
                }
                temp_label = temp_label->next;
            }
        }
    }
}

void createOutputFiles (char* file_name, LabelNode* labels, Error* error) {
    createFileWithLabelType(file_name, labels, LABEL_TYPE_ENTRY ,error);
    createFileWithLabelType(file_name, labels, LABEL_TYPE_EXTERNAL ,error);
}

void createFileWithLabelType(char* file_name, LabelNode* labels, LabelType label_type, Error* error) {
    FILE *fptr;
    char output_file_name[MAX_FILE_NAME_WITH_EXTENSION];
    
    cleanLine(output_file_name, MAX_FILE_NAME_WITH_EXTENSION);

    strcat(output_file_name, "output/");
    strcat(output_file_name, file_name);
    switch (label_type) {
        case LABEL_TYPE_ENTRY:
            strcat(output_file_name, ".ent");
            break;
        case LABEL_TYPE_EXTERNAL:
            strcat(output_file_name, ".ext");
            break;
        default:
            /*Error*/
            break;
    }

    fptr = fopen(output_file_name, "w");

    if (!fptr) {
        /*error*/
        return;
    }

    while (labels) {
        if (labels->label_type == label_type) {
            fprintf(fptr, "%s %d\n", labels->label_name, labels->memory_adress);
        }
        labels = labels->next;
    }
    fclose(fptr);
    return;
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
        if (colon && word[i] == ':' && word[i+1] == '\0')
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


LabelType getLabelType(char* label, LabelNode* LabelPtr){
    if (LabelPtr != NULL)
    {
        do
        {
            if (!strcmp(label, LabelPtr->label_name))
            {
                printf("found Label %s is of type: %d\n", label, LabelPtr->label_type);
                return LabelPtr->label_type;
            }
            LabelPtr = LabelPtr->next;
        }
        while (LabelPtr != NULL);
    }
    
    printf("error, haven't found the label %s\n", label);
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

    /* Check for a minus sign at the beginning*/
    if (word[i] == '-')
        i++;  /* Skip the minus sign */
    
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

void insertNewLabel(LabelNode** labels, char* label_name, LabelType label_type, int* memory_idx, Error* error) {
    LabelNode* temp_label;
    LabelNode* new_label;
    temp_label = *labels;
    if (temp_label) {
        while(temp_label && temp_label->next) {
            temp_label = temp_label->next;
        }
        new_label = (LabelNode*) allocateMemory(sizeof(LabelNode), error);
        new_label->label_name = label_name;
        new_label->label_type = label_type;
        new_label->memory_adress = *memory_idx;
        temp_label->next = new_label;
    } else {
        *labels = (LabelNode*) allocateMemory(sizeof(LabelNode), error);
        (*labels)->label_name = label_name;
        (*labels)->label_type = label_type;
        (*labels)->memory_adress = *memory_idx;
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
            return commands[i].opcode;
        }
        
    }
    return -1;
}

int checkCommandLine(char** tokens, int num_tokens, bool label){
    short opcode = checkCommand(tokens[label]);
    int count = 0;
    int operand_index = label+1; /*operand index*/
    int operand_result = -1;
    bool register_flag = false;
    bool source_flag = true; /* flag that looks after the operand if its a source or destination*/

    int L = 1;

    /*ERROR unrecognized command name*/
    if (opcode == -1)
    {
        return COMMAND_LINE_ERROR;
    }

    /* ERROR wrong amount of operands for the command */
    if ((num_tokens <= 2 + label) && (num_tokens - label - 1) != commands[opcode].number_of_operands)
    {
        return COMMAND_LINE_ERROR;
    }
    
    /*ERROR illegal comma*/
    if (!strcmp(tokens[num_tokens-1], ","))
    {
        printf("ERROR ILLEGAL COMMA HERE:");
        printLine(tokens, num_tokens);
        return COMMAND_LINE_ERROR;
    }
    
    if (num_tokens > 2 + label && (num_tokens - label - 2) != commands[opcode].number_of_operands)
    {
        printf("ERROR INCORRECT NUMBER OF OPERANDS HERE: ");
        printLine(tokens, num_tokens);
        return COMMAND_LINE_ERROR;
    }
    

    for (; count < commands[opcode].number_of_operands; count++)
    {
        if (!source_flag)
        {
            operand_result = checkOperand(tokens[operand_index + count + 1]);
        }
        else
        {           
            operand_result = checkOperand(tokens[operand_index + count]);
        }
        
        switch (operand_result)
        {
        case OPERAND_TYPE_LABEL:
            L++;
            if (commands[opcode].number_of_operands < 2)
            {
                if (!commands[opcode].destinationAddresingMethod[ADDRESING_LABEL])
                {
                    printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                    return COMMAND_LINE_ERROR;
                }
                else
                {
                    break;
                }
            }
            
            if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_LABEL]) || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_LABEL]))
            {
                printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                return COMMAND_LINE_ERROR; 
            }
            
            break;

        case OPERAND_TYPE_REGISTER:

            if (commands[opcode].number_of_operands < 2)
            {
                if (!commands[opcode].destinationAddresingMethod[ADDRESING_REGISTER])
                {
                    printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                    return COMMAND_LINE_ERROR;
                }
                else
                {
                    break;
                }
            }

            if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_REGISTER]) || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_REGISTER]))
            {
                printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                return COMMAND_LINE_ERROR; 
            }

            if (!register_flag)
            {
                L++;
                register_flag = true;
            }
            break;

        case OPERAND_TYPE_NUMBER:
            if (commands[opcode].number_of_operands < 2)
            {
                if (!commands[opcode].destinationAddresingMethod[ADDRESING_NUMBER])
                {
                    printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                    return COMMAND_LINE_ERROR;
                }
                else
                {
                    L++;
                    break;
                }
            }

            if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_NUMBER]) || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_NUMBER]))
            {
                printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                return COMMAND_LINE_ERROR; 
            }

            L++;
            break;

        default:
            printf("ERROR IN OPERAND %s\n", tokens[operand_index + count]);
            return COMMAND_LINE_ERROR;
            break;
        }
        
        if (source_flag)
        {
            source_flag = false;
        }
        operand_result = -1;
    }
    printf("CORRECT COMMAND LINE: %s\n", *tokens);
    return L;
}

OperandType checkOperand(char* operand){
    const char* registers[] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};
    int i = 0;

    for (; i < NUM_OF_REGISTERS; i++)
    {
        if (!strcmp(registers[i], operand))
        {
            printf("THE OPERAND %s is of type: %d\n", operand, OPERAND_TYPE_REGISTER);
            return OPERAND_TYPE_REGISTER;
        }
        
    }
    
    if (isNumber(operand))
    {
        printf("THE OPERAND %s is of type: %d\n", operand, OPERAND_TYPE_NUMBER);
        return OPERAND_TYPE_NUMBER;
    }
    
    if (isLabel(operand, false))
    {
        return OPERAND_TYPE_LABEL;
    }

    /*handle error*/
    printf("ILLEGAL        OPERAND: %s\n", operand);    
    return OPERAND_TYPE_OTHER;
}

void printLine(char** tokens, int num_tokens){
    int i = 0;
    for (; i < num_tokens; i++)
    {
        printf("%s ",tokens[i]);
    }
    printf("\n");
}