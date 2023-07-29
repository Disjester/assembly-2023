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

static const char base64_chars[64] = {"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"};

void firstIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode** labels, int* DC, int* IC, Error* error) {
    bool is_first_itteration_flag = true;
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
    short data[100];
    short binary_word;
    int L = 0;
    int num_line = 1;
    char test_base64[3];

    if (*error == ERROR_MEMORY_ALLOCATION) return;
 
    printf("!!!   BEGGINING OF THE FIRST ITERATION   !!!\n\n");
    *DC = *IC = 0;
    cleanMemory(memory);
    temp_code = code;
    while(temp_code) {
        if (temp_code->code_row[0] == ';') {
            temp_code = temp_code->next;
            num_line++;
            continue;
        }
        if (temp_code->code_row[0] == '\n' || temp_code->code_row[0] == '\0') {
            temp_code = temp_code->next;
            num_line++;
            continue;
        }
        tokenizeInput(temp_code->code_row, tokens, &num_tokens, error);
        if (*error == ERROR_MEMORY_ALLOCATION) return;

        if(isLabel(tokens[token_idx], true)) {
            label_flag = true;
            token_idx++;
        }
         
        switch (isDotType(tokens[token_idx], error)) {
            case DOT_DATA:
                if (label_flag) {
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, DC, error);
                    if (*error == ERROR_MEMORY_ALLOCATION) return;
                    test_label_node = *labels;
                    printf("CURRENT  LABEL  TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag, error)) {
                    printf("I   SEE   DATA   HERE: %s\n", temp_code->code_row);
                    token_idx++;
                    for (i = token_idx; i < num_tokens; i += 2) {
                        pushToMemory(memory_idx, memory, atoi(tokens[i]));
                        (*DC)++;
                    }
                    printf("CURRENT   IC  AND  DC: %d, %d\n", *IC, *DC);
                }
                /*errr handaling*/
                if (*error != NO_ERROR)
                {
                    handleError(error, num_line);
                    *error = NO_ERROR;
                    if (temp_code->next != NULL)
                    {
                        temp_code =  temp_code->next;
                    }
                    continue;
                }
                printf("CURRENT        MEMORY: ");
                for (i = 100; i < *memory_idx; i++) {
                    convertToBase64(memory[i], test_base64);
                    printf("%d:%s ", i, test_base64);
                }
                printf("\n");
                break;
            case DOT_STRING:
                if (label_flag) {
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, DC, error);
                    if (*error == ERROR_MEMORY_ALLOCATION) return;
                    test_label_node = *labels;
                    printf("CURRENT  LABEL  TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkDataLine(tokens, num_tokens, label_flag, error)) {
                    /*printf("I  SEE  STRING   HERE: %s\n", temp_code->code_row);*/
                    token_idx++;
                    for (i = 1; i < (strlen(tokens[token_idx])-1); i++) {
                        pushToMemory(memory_idx, memory, tokens[token_idx][i]);
                        (*DC)++;
                    }
                    pushToMemory(memory_idx, memory, '\0');
                    (*DC)++;
                    /*printf("CURRENT  IC   AND  DC: %d, %d\n", *IC, *DC);*/
                }
                /*handlaing Error*/
                if (*error != NO_ERROR)
                {
                    handleError(error, num_line);
                    *error = NO_ERROR;
                    if (temp_code->next != NULL)
                    {
                        temp_code =  temp_code->next;
                    }
                    continue;
                }
                printf("CURRENT        MEMORY: ");
                for (i = 100; i < *memory_idx; i++) {
                    convertToBase64(memory[i], test_base64);
                    printf("%d:%s ", i, test_base64);
                }
                printf("\n");
                break;
            case DOT_EXTERN:
                place = 1;
                for (; place < num_tokens; place++) {
                    if (isLabel(tokens[place], false)) {
                        insertNewLabel(labels, tokens[place], LABEL_TYPE_EXTERNAL, &def_extern_mem, error);
                        if (*error == ERROR_MEMORY_ALLOCATION) return;
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
                    if (*error == ERROR_MEMORY_ALLOCATION) return;
                    test_label_node = *labels;
                    printf("CURRENT  LABEL  TABLE: ");
                    while (test_label_node) {
                        printf("%s:%d ", test_label_node->label_name, test_label_node->memory_adress);
                        test_label_node = test_label_node->next;
                    }
                    printf("\n");
                }
                if (checkCommandLine(tokens, num_tokens, label_flag, *labels, error, is_first_itteration_flag) != COMMAND_LINE_ERROR)
                {
                    binary_word = createCommandBinaryWord(tokens, num_tokens, token_idx, error, is_first_itteration_flag, *labels);
                    pushToMemory(memory_idx, memory, binary_word);
                    L = checkCommandLine(tokens, num_tokens, label_flag, *labels, error, is_first_itteration_flag);
                    createOperandBinaryWord(L, *labels, true, checkOperand(tokens[token_idx + 1], *labels, error, is_first_itteration_flag), checkOperand(tokens[token_idx + 3], *labels, error, is_first_itteration_flag), tokens[token_idx + 1], tokens[token_idx + 3], memory_idx, memory, error);
                }
                /*handle error*/
                if (*error != NO_ERROR)
                {
                    handleError(error, num_line);
                    *error = NO_ERROR;
                    if (temp_code->next != NULL)
                    {
                        temp_code =  temp_code->next;
                    }
                    continue;
                }
                *IC += L;
                L = 0;
                printf("THE IC IS : %d\n",*IC);
                break;
        }
        token_idx = 0;
        label_flag = false;
        num_line++;
        temp_code = temp_code->next;
    }

    temp_label_node = *labels;
    while (temp_label_node) {
        switch (temp_label_node->label_type) {
            case LABEL_TYPE_DATA:
                temp_label_node->memory_adress += *IC;
            case LABEL_TYPE_CODE:
            case LABEL_TYPE_ENTRY:
            case LABEL_TYPE_EXTERNAL:
                temp_label_node->memory_adress += 100;
                break;
            case LABEL_TYPE_NOT_FOUND:
                /* *error = ERROR_UNRECOGNIZED_LABEL; */
                break;
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

void createOperandBinaryWord(int L, LabelNode* labels, bool is_first_iteration, OperandType op_type_1, OperandType op_type_2, char* operand1, char* operand2, int* memory_idx, short* memory, Error* error) {
    short resulting_binary_word = 0x0;
    LabelNode* temp_label_node;

    switch (L) {
        case 1:
            break;
        case 2:
            if (op_type_1 == OPERAND_TYPE_REGISTER && op_type_2 == OPERAND_TYPE_REGISTER) {
                resulting_binary_word += (short) atoi(operand1 + 2);
                resulting_binary_word <<= 5;
                resulting_binary_word += (short) atoi(operand2 + 2);
                resulting_binary_word <<= 2;
                pushToMemory(memory_idx, memory, resulting_binary_word);
            } else {
                switch (op_type_1) {
                    case OPERAND_TYPE_REGISTER:
                        resulting_binary_word += (short) atoi(operand1 + 2);
                        resulting_binary_word <<= 7;
                        pushToMemory(memory_idx, memory, resulting_binary_word);
                        break;
                    case OPERAND_TYPE_LABEL:
                        if (!is_first_iteration) {
                            temp_label_node = labels;
                            while (temp_label_node) {
                                if (!strcmp(temp_label_node->label_name, operand1)) {
                                    resulting_binary_word += temp_label_node->memory_adress;
                                    resulting_binary_word <<= 2;
                                    pushToMemory(memory_idx, memory, resulting_binary_word);
                                }
                                temp_label_node = temp_label_node->next;
                            }
                        } else {
                            pushToMemory(memory_idx, memory, 0xFFF);
                        }
                        break;
                    case OPERAND_TYPE_NUMBER:
                        resulting_binary_word += (short) atoi(operand1);
                        resulting_binary_word <<= 2;
                        pushToMemory(memory_idx, memory, resulting_binary_word);
                        break;
                    case OPERAND_TYPE_OTHER:
                        break;
                }
            }
            break;
        case 3:
            switch (op_type_1) {
                case OPERAND_TYPE_REGISTER:
                    resulting_binary_word += (short) atoi(operand1 + 2);
                    resulting_binary_word <<= 7;
                    pushToMemory(memory_idx, memory, resulting_binary_word);
                    break;
                case OPERAND_TYPE_LABEL:
                    if (!is_first_iteration) {
                        temp_label_node = labels;
                        while (temp_label_node) {
                            if (!strcmp(temp_label_node->label_name, operand1)) {
                                resulting_binary_word += temp_label_node->memory_adress;
                                resulting_binary_word <<= 2;
                                pushToMemory(memory_idx, memory, resulting_binary_word);
                            }
                            temp_label_node = temp_label_node->next;
                        }
                    } else {
                        pushToMemory(memory_idx, memory, 0xFFF);
                    }
                    break;
                case OPERAND_TYPE_NUMBER:
                    resulting_binary_word += (short) atoi(operand1);
                    resulting_binary_word <<= 2;
                    pushToMemory(memory_idx, memory, resulting_binary_word);
                    break;
                case OPERAND_TYPE_OTHER:
                    break;
            }
            switch (op_type_2) {
                case OPERAND_TYPE_REGISTER:
                    resulting_binary_word += (short) atoi(operand2 + 2);
                    resulting_binary_word <<= 7;
                    pushToMemory(memory_idx, memory, resulting_binary_word);
                    break;
                case OPERAND_TYPE_LABEL:
                    if (!is_first_iteration) {
                        temp_label_node = labels;
                        while (temp_label_node) {
                            if (!strcmp(temp_label_node->label_name, operand2)) {
                                resulting_binary_word += temp_label_node->memory_adress;
                                resulting_binary_word <<= 2;
                                pushToMemory(memory_idx, memory, resulting_binary_word);
                            }
                            temp_label_node = temp_label_node->next;
                        }
                    } else {
                        pushToMemory(memory_idx, memory, 0xFFF);
                    }
                    break;
                case OPERAND_TYPE_NUMBER:
                    resulting_binary_word += (short) atoi(operand2);
                    resulting_binary_word <<= 2;
                    pushToMemory(memory_idx, memory, resulting_binary_word);
                    break;
                case OPERAND_TYPE_OTHER:
                    break;
            }
            break;
    }resulting_binary_word <<= 2;
}

short createCommandBinaryWord(char** tokens, int num_tokens, int token_idx, Error* error, bool is_first_itteration, LabelNode* labelPtr) {
    short resulting_binary_word = 0;
    short source_operand, destination_operand;
    int temp_idx = token_idx;
    int operand_amount;
    int opcode;

    operand_amount = getOperandAmount(tokens[temp_idx]);
    opcode = checkCommand(tokens[temp_idx++]);

    switch (operand_amount) {
        case 0:
            source_operand = destination_operand = 0x0;
            break;
        case 1:
            destination_operand = getAdressingMethodByOperandType(checkOperand(tokens[temp_idx++], labelPtr, error, is_first_itteration));;
            source_operand = 0x0;
            break;
        case 2:
            source_operand = getAdressingMethodByOperandType(checkOperand(tokens[temp_idx++], labelPtr, error, is_first_itteration));
            destination_operand = getAdressingMethodByOperandType(checkOperand(tokens[++temp_idx], labelPtr, error, is_first_itteration));
            break;
    }
    resulting_binary_word +=  source_operand;
    resulting_binary_word <<= 4;
    resulting_binary_word +=  opcode;
    resulting_binary_word <<= 3;
    resulting_binary_word += destination_operand;
    resulting_binary_word <<= 2;
    resulting_binary_word += 0; /*A.R.E. CHANGE IT, BORIS*/
    return resulting_binary_word;
}

void convertToBase64(short num, char* result) {
    result[0] = base64_chars[(num >> 6) & 0x3F];
    result[1] = base64_chars[num & 0x3F];
    result[2] = '\0';
}

int getAdressingMethodByOperandType(OperandType operand_type) {
    switch (operand_type) {
        case OPERAND_TYPE_NUMBER:   return 0x1; /*TO BE CHANGED TO CONSTANT*/
        case OPERAND_TYPE_LABEL:    return 0x3;
        case OPERAND_TYPE_REGISTER: return 0x5;
        case OPERAND_TYPE_OTHER:    return 0xFFF;
    }
    return -1;
}

int getOperandAmount(char* command) {
    int i;
    for(i = 0; i < NUMBER_OF_COMMANDS; i++) {
        if (!strcmp(command, commands[i].command)) {
            return commands[i].number_of_operands;
        }
    }
    return -1; /*CHANGE LATER*/
}

void secondIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode* labels, int* DC, int* IC, Error* error, char* file_name) {
    CodeNode* temp_code;
    bool is_first_itteration_flag = false; 
    int token_idx = 0;
    bool label_flag = false;
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), error);
    int num_tokens = 0;
    int L = 0;
    int num_line = 0;

    printf("!!!   BEGGINING OF THE SECOND ITERATION   !!!\n\n");
    temp_code = code;
    *IC = 0;
    while (temp_code) {
        num_line++;
        tokenizeInput(temp_code->code_row, tokens, &num_tokens, error);
        if(isLabel(tokens[token_idx], true)) {
            /*printf("I  SEE   LABEL   HERE: %s\n",temp_code->code_row);*/
            label_flag = true;
            token_idx++;
        }
        switch (isDotType(tokens[token_idx], error)) {
            case DOT_ENTRY:
                updateEntryLabels(labels, tokens, num_tokens, token_idx);
                break;
            case DOT_OTHER:
                if (checkCommandLine(tokens, num_tokens, label_flag, labels, error, is_first_itteration_flag) != COMMAND_LINE_ERROR)
                {
                    L = checkCommandLine(tokens, num_tokens, label_flag, labels, error, is_first_itteration_flag);
                }
                if (*error != NO_ERROR)
                {
                    handleError(error, num_line);
                    *error = NO_ERROR;
                    if (temp_code->next != NULL)
                    {
                        temp_code =  temp_code->next;
                    }
                    continue;
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
    createOutputFiles(file_name, labels, memory, memory_idx, error);
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

void createOutputFiles (char* file_name, LabelNode* labels, short* memory, int* memory_idx, Error* error) {
    createFileWithLabelType(file_name, labels, LABEL_TYPE_ENTRY ,error);
    createFileWithLabelType(file_name, labels, LABEL_TYPE_EXTERNAL ,error);
    createFileWithMemoryDump(file_name, memory, memory_idx);
}

void createFileWithMemoryDump(char* file_name, short* memory, int* memory_idx) {
    FILE *fptr;
    int i;
    char output_file_name[MAX_FILE_NAME_WITH_EXTENSION];
    char base64[3];

    cleanLine(output_file_name, MAX_FILE_NAME_WITH_EXTENSION);
    strcat(output_file_name, "output/");
    strcat(output_file_name, file_name);
    strcat(output_file_name, ".ob");

    fptr = fopen(output_file_name, "w");

    if (!fptr) {
        /*ERROR*/
        return;
    }

    for (i = 100; i < *memory_idx; i++) { /*MAGIC NUMBER*/
        convertToBase64(memory[i], base64);
        fprintf(fptr, "%s\n", base64);
    }
    fclose(fptr);
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
            /* *error = ERROR_UNRECOGNIZED_LABEL;*/
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


short isDotType(char* word, Error* error){
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


LabelType getLabelType(char* label, LabelNode* LabelPtr, Error* error){
    
    if (LabelPtr != NULL)
    {
        do
        {
            if (!strcmp(label, LabelPtr->label_name))
            {
                /*printf("found Label %s is of type: %d\n", label, LabelPtr->label_type);*/
                return LabelPtr->label_type;
            }
            LabelPtr = LabelPtr->next;
        }
        while (LabelPtr != NULL);
    }
    
    *error = ERROR_UNRECOGNIZED_LABEL;
    /*printf("error, haven't found the label %s\n", label);*/
    return LABEL_TYPE_NOT_FOUND;

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
            return false; /* not a number*/
        }
        
    }
    return true;
    
}

bool checkDataLine(char** tokens, int num_tokens, bool label, Error* error){
    
    int token_index = 1;
    
    if (num_tokens < (2 + label))
    {
        *error = ERROR_MISSING_DATA_ARGUMENT;
        /*printf("The line is missing arguments\n");*/
        return false;
    }
    
    if (isDotType(tokens[0 + label], error) == DOT_STRING)
    {
        if (num_tokens > (2 + label))
        {
            /*printf("too many arguments\n");*/
            *error = ERROR_EXTRANEOS_TEXT;
            return false;
        }
        if (isString(tokens[1 + label]))
        {
            return true;
        }
        
    }
    
    if (isDotType(tokens[0 + label], error) == DOT_DATA)
    {
        if (num_tokens % 2 == (1 + label))
        {
            *error = ERROR_WRONG_NUM_OF_COMMAS;
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
                *error = ERROR_MISSING_COMMA;
                printf("Missing a ,");
                return false;
            }
        }
        return true;
    }

    return false;
}

void pushToMemory(int* memory_idx, short* memory, short memoryField) {
    if (memory_idx >= 1024) /* change from magic number*/
    {
        *error = ERROR_MAXED_OUT_MEMORY;
        return;
    }
    
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

int checkCommandLine(char** tokens, int num_tokens, bool label, LabelNode* LabelPtr, Error* error, bool is_first_iteration){
    
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
        *error = ERROR_WRONG_AMOUNT_OF_OPERANDS;
        return COMMAND_LINE_ERROR;
    }
    
    /*ERROR illegal comma*/
    if (!strcmp(tokens[num_tokens-1], ","))
    {
        *error = ERROR_ILLEGAL_COMMA;
        printf("ERROR ILLEGAL COMMA HERE:");
        printLine(tokens, num_tokens);
        return COMMAND_LINE_ERROR;
    }
    
    if (num_tokens > 2 + label && (num_tokens - label - 2) != commands[opcode].number_of_operands)
    {
        *error = ERROR_WRONG_AMOUNT_OF_OPERANDS;
        printf("ERROR INCORRECT NUMBER OF OPERANDS HERE: ");
        printLine(tokens, num_tokens);
        return COMMAND_LINE_ERROR;
    }
    

    for (; count < commands[opcode].number_of_operands; count++)
    {
        if (!source_flag)
        {
            operand_result = checkOperand(tokens[operand_index + count + 1], LabelPtr, error, is_first_iteration);
        }
        else
        {           
            operand_result = checkOperand(tokens[operand_index + count], LabelPtr, error, is_first_iteration);
        }
        
        switch (operand_result)
        {
        case OPERAND_TYPE_LABEL:
            L++;
            if (commands[opcode].number_of_operands < 2)
            {
                if (!commands[opcode].destinationAddresingMethod[ADDRESING_LABEL])
                {
                    *error = ERROR_INCORRECT_OPERAND_TYPE;
                    printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                    return COMMAND_LINE_ERROR;
                }
                break;
            }
            
            if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_LABEL]) || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_LABEL]))
            {
                *error = ERROR_INCORRECT_OPERAND_TYPE;
                printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                return COMMAND_LINE_ERROR; 
            }
            
            break;

        case OPERAND_TYPE_REGISTER:

            if (commands[opcode].number_of_operands < 2)
            {
                if (!commands[opcode].destinationAddresingMethod[ADDRESING_REGISTER])
                {
                    *error = ERROR_INCORRECT_OPERAND_TYPE;
                    printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                    return COMMAND_LINE_ERROR;
                }
                else
                {
                    L++;
                    break;
                }
            }

            if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_REGISTER]) || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_REGISTER]))
            {
                *error = ERROR_INCORRECT_OPERAND_TYPE;
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
                    *error = ERROR_INCORRECT_OPERAND_TYPE;
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
                *error = ERROR_INCORRECT_OPERAND_TYPE;
                printf("INCORRECT OPERAND FOR %s: %s\n",commands[opcode].command, tokens[operand_index + count]);
                return COMMAND_LINE_ERROR; 
            }

            L++;
            break;

        default:
            *error = ERROR_ILLEGAL_OPERAND_TYPE;
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
    return L;
}

OperandType checkOperand(char* operand, LabelNode* LabelPtr, Error* error, bool is_first_iteration){
    
    const char* registers[] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"};
    int i = 0;

    
    for (; i < NUM_OF_REGISTERS; i++)
    {
        if (!strcmp(registers[i], operand))
        {
            /*printf("THE OPERAND %s is of type: %d\n", operand, OPERAND_TYPE_REGISTER);*/
            return OPERAND_TYPE_REGISTER;
        }
        
    }
    
    if (isNumber(operand))
    {
        /*printf("THE OPERAND %s is of type: %d\n", operand, OPERAND_TYPE_NUMBER);*/
        return OPERAND_TYPE_NUMBER;
    }
    
    if (is_first_iteration)
    {
        if (isLabel(operand, false))
        {
            return OPERAND_TYPE_LABEL;
        }
    }

    else if (getLabelType(operand, LabelPtr, error) != LABEL_TYPE_NOT_FOUND)
    {
        return OPERAND_TYPE_LABEL;
    }
    

    /*handle error*/
    
    /*printf("ILLEGAL        OPERAND: %s\n", operand);*/
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