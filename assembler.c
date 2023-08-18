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

/**
 * @brief Performs the first iteration of the assembler.
 * 
 * @param memory Global memory array.
 * @param memory_idx Index in the global memory array.
 * @param code Pointer to the code linked list.
 * @param labels Pointer to the labels linked list.
 * @param DC Data Counter.
 * @param IC Instruction counter.
 * @param is_print Pointer to a boolean indicating whether to create output files
 * @param error Pointer to an Error variable for error handling.
 */
void firstIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode** labels, int* DC, int* IC,
bool* is_print, Error* error) {
    bool is_first_itteration_flag = true; /* Flag indicating the first iteration of the assembler */
    bool stop_flag = false; /* Flag indicating if a "stop" command has been encountered */
    CodeNode* temp_code; /* Pointer to traverse the code linked list */
    LabelNode* temp_label_node; /* Temporary pointer to a label in the labels linked list */
    bool label_flag = false; /* Flag indicating if a label is present */
    int data_memory_idx = DEFAULT_MEMORY_INDEX; /* Index for data memory */
    int operand_num = 0; /* Number of operands for a command */
    int i; /* Loop iterator */
    int def_extern_mem = DEFAULT_EXTERN_MEMORY; /* Default external memory address */
    int num_tokens = DEFAULT_VALUE; /* Number of tokens in the current code row */
    int token_idx = DEFAULT_VALUE; /* Index of the current token */
    short binary_word; /* Binary word representation of an instruction */
    int L = DEFAULT_VALUE; /* Length of the current command */
    int num_line = STARTING_LINE; /* Current line number */
    short data_memory[MAX_MEMORY_SIZE]; /* Array to hold data memory values */
    int char_index = 1; /* Index of the current character in a string */
    int token_len = 0; /* Length of the current token */
    bool end_of_string_flag = false; /* Flag indicating the end of a string */
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), is_print, error); /* Array of pointers to tokens */
    allocateMemoryTokens(tokens, is_print, error); /* Allocate memory for tokens array */

    if (*error == ERROR_MEMORY_ALLOCATION) return; /* Return if memory allocation fails */

    *DC = *IC = DEFAULT_VALUE; /* Initialize Data Counter and Instruction Counter */
    cleanMemory(memory); /* Clear the global memory array */
    cleanMemory(data_memory); /* Clear the data memory array */
    temp_code = code; /* Initialize temp_code to point to the beginning of the code linked list */

    
    while(temp_code) {
        token_idx = DEFAULT_VALUE; /* Reset the index of the current token */
        end_of_string_flag = false; /* Reset the end of string flag */
        char_index = 1; /* Reset the index of the current character in a string */
        label_flag = false; /* Reset the label flag */

        /* Check if the current line is empty or a comment, and skip to the next line if true */
        if (temp_code->code_row[FIRST_CHARACTER] == '\n' || temp_code->code_row[FIRST_CHARACTER] == '\0' 
        || temp_code->code_row[FIRST_CHARACTER] == '\r' || temp_code->code_row[FIRST_CHARACTER] == ';') {
            temp_code = temp_code->next; /* Move to the next code line */
            num_line++; /* Increment the line number */
            continue; /* Skip the rest of the loop */
        }

        /* Tokenize the current code line */
        tokenizeInput(temp_code->code_row, tokens, &num_tokens, is_print, error);

        /* Check for memory allocation error during tokenization */
        if (*error != NO_ERROR) {
            freeMemory(tokens, code, NULL, NULL); /* Free allocated memory */
            return; /* Return from the function */
        }

        /* Check if the current token is a label */
        if(isLabel(tokens[token_idx], true)) {
            label_flag = true; /* Set the label flag to true */
            token_idx++; /* Move to the next token */
        }

        switch (getDotType(tokens[token_idx], error)) {
            case DOT_DATA:
                if (label_flag) {
                    /* Insert a new data label into the labels linked list */
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, DC, is_print,
                    error, is_first_itteration_flag);
                    /* Handle memory allocation error */
                    if (*error == ERROR_MEMORY_ALLOCATION) {
                        handleError(error, num_line, is_print);
                        return; /* Return from the function */
                    }
                    /* Handle duplicate label error */
                    if (*error == ERROR_DUPLICATE_LABEL)
                    {
                        handleError(error, num_line, is_print);
                        *error = NO_ERROR;
                        moveToNextCodeLine(&temp_code, &num_line);
                        continue; /* Skip the rest of the loop iteration */
                    }
                }
                /* Check and process the data tokens */
                if (checkDataLine(tokens, num_tokens, label_flag, error)) {
                    token_idx++;
                    for (i = token_idx; i < num_tokens; i += 2) {
                        /* Convert and push the data value to the data memory */
                        pushToMemory(&data_memory_idx, data_memory, atoi(tokens[i]), error, num_line, is_print);
                        /* Handle memory usage reaching its limit */
                        if (*error == ERROR_MAXED_OUT_MEMORY) return; /* Return from the function */
                        (*DC)++; /* Increment the Data Counter */
                    }
                }
                /* Handle data-related errors */
                if (*error != NO_ERROR) {
                    handleError(error, num_line, is_print);
                    *error = NO_ERROR;
                    moveToNextCodeLine(&temp_code, &num_line);
                    continue; /* Skip the rest of the loop iteration */
                }
                break;
        
                case DOT_STRING:
                    if (label_flag) {
                        /* Insert a new data label into the labels linked list */
                        insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_DATA, DC, is_print,
                        error, is_first_itteration_flag);
                        /* Handle memory allocation error */
                        if (*error == ERROR_MEMORY_ALLOCATION) {
                            handleError(error, num_line, is_print);
                            return; /* Return from the function */
                        }
                        /* Handle duplicate label error */
                        if (*error == ERROR_DUPLICATE_LABEL)
                        {
                            handleError(error, num_line, is_print);
                            *error = NO_ERROR;
                            moveToNextCodeLine(&temp_code, &num_line);
                            continue; /* Skip the rest of the loop iteration */
                        }                    
                    }
                    /* Check and process the data tokens as a string */
                    if (checkDataLine(tokens, num_tokens, label_flag, error)) {
                        token_idx++;
                        for (; token_idx < num_tokens; token_idx++)
                        {
                            token_len = strlen(tokens[token_idx]) + 1;
                            for (; char_index < token_len; char_index++) {
                                /* Check for the end of the string */
                                if (tokens[token_idx][char_index] == '"'){
                                    end_of_string_flag = true;
                                    break; /* Exit the inner loop */
                                }
                                /* Replace null terminators with spaces */
                                else if (tokens[token_idx][char_index] == '\0')
                                {
                                    tokens[token_idx][char_index] = ' ';
                                }
                                /* Push each character to the data memory */
                                pushToMemory(&data_memory_idx, data_memory, tokens[token_idx][char_index], error,
                                num_line, is_print);
                                /* Handle memory usage reaching its limit */
                                if (*error == ERROR_MAXED_OUT_MEMORY) return; /* Return from the function */
                                (*DC)++; /* Increment the Data Counter */
                            }
                            /* Exit the outer loop if the end of the string is encountered */
                            if (end_of_string_flag) break;
                            char_index = 0; /* Reset char_index for the next token */
                        }
                        /* Push null terminator to mark the end of the string */
                        pushToMemory(&data_memory_idx, data_memory, '\0', error, num_line, is_print);
                        /* Handle memory usage reaching its limit */
                        if (*error == ERROR_MAXED_OUT_MEMORY) return; /* Return from the function */
                        (*DC)++; /* Increment the Data Counter */
                    }
                    /* Handle data-related errors */
                    if (*error != NO_ERROR) {
                        handleError(error, num_line, is_print);
                        *error = NO_ERROR;
                        moveToNextCodeLine(&temp_code, &num_line);
                        continue; /* Skip the rest of the loop iteration */
                    }
                    break; /* End of case DOT_STRING */

            case DOT_EXTERN:
                /* Iterate through tokens starting from index 1 */
                for (i = 1; i < num_tokens; i++) {
                    /* Check if the token is a label */
                    if (isLabel(tokens[i], false)) {
                        /* Insert a new external label into the labels linked list */
                        insertNewLabel(labels, tokens[i], LABEL_TYPE_EXTERNAL, &def_extern_mem, is_print, error,
                        is_first_itteration_flag);
                        /* Handle memory allocation error */
                        if (*error == ERROR_MEMORY_ALLOCATION) return; /* Return from the function */
                        /* Handle duplicate label error */
                        if (*error == ERROR_DUPLICATE_LABEL)
                        {
                            /* Clear the error and skip the rest of the loop iteration */
                            /* handleError(error, num_line, is_print); */
                            *error = NO_ERROR;
                            moveToNextCodeLine(&temp_code, &num_line);
                            continue; /* Skip the rest of the loop iteration */
                        }                        
                    }
                    else {
                        break; /* Exit the loop if a non-label token is encountered */
                    }
                }
                break; /* End of case DOT_EXTERN */

            case DOT_ENTRY:
                break; /* End of case DOT_ENTRY */

            case DOT_OTHER:
                /* Check if stop_flag is set */
                if (stop_flag) {
                    /* Set error and handle it */
                    *error = ERROR_CODE_AFTER_STOP;
                    handleError(error, num_line, is_print);
                }
                /* Check if there is a label */
                if (label_flag) {
                    /* Insert a new label into the labels linked list with LABEL_TYPE_CODE */
                    insertNewLabel(labels, removeColon(tokens[token_idx-1]), LABEL_TYPE_CODE, IC, is_print, error,
                    is_first_itteration_flag);
                    /* Handle memory allocation error */
                    if (*error == ERROR_MEMORY_ALLOCATION){
                        handleError(error, num_line, is_print);
                        return; /* Return from the function */
                    }
                    /* Handle duplicate label error */
                    if (*error == ERROR_DUPLICATE_LABEL)
                    {
                        handleError(error, num_line, is_print);
                        *error = NO_ERROR;
                        moveToNextCodeLine(&temp_code, &num_line);                        
                        continue; /* Skip the rest of the loop iteration */
                    }                    
                }
                /* Check the command line and get L value */
                if (checkCommandLine(tokens, num_tokens, label_flag, *labels, error, is_first_itteration_flag, &stop_flag,
                is_print, num_line) != COMMAND_LINE_ERROR) {
                    /* Create the binary word and push it to memory */
                    binary_word = createCommandBinaryWord(tokens, num_tokens, token_idx, error, is_first_itteration_flag,
                    *labels);
                    pushToMemory(memory_idx, memory, binary_word, error, num_line, is_print);
                    /* Handle memory allocation error */
                    if (*error == ERROR_MAXED_OUT_MEMORY) return; /* Return from the function */
                    /* Get L value and calculate operand_num */
                    L = checkCommandLine(tokens, num_tokens, label_flag, *labels, error, is_first_itteration_flag, &stop_flag,
                    is_print, num_line);
                    if (num_tokens >= 4)
                    {
                        operand_num = 2;
                    }
                    else
                    {
                        operand_num = L-1;
                    }
                    /* Calculate operand_num and create operand binary word */
                    switch (operand_num)
                    {
                    case 0:
                        /* No operands */
                        createOperandBinaryWord(L, *labels, true, OPERAND_TYPE_OTHER, OPERAND_TYPE_OTHER, (char*) NULL,
                        (char*) NULL,
                        memory_idx, memory, error, num_line, is_print);
                        break;
                    case 1:
                        /* One operand */
                        createOperandBinaryWord(L, *labels, true, checkOperand(tokens[token_idx + 1], *labels, error,
                        is_first_itteration_flag),
                        OPERAND_TYPE_OTHER, tokens[token_idx + 1], (char*) NULL, memory_idx, memory, error, num_line, is_print);
                        break;
                    case 2:
                        /* Two operands */
                        createOperandBinaryWord(L, *labels, true, checkOperand(tokens[token_idx + 1], *labels, error,
                        is_first_itteration_flag),
                        checkOperand(tokens[token_idx + 3], *labels, error, is_first_itteration_flag), tokens[token_idx + 1],
                        tokens[token_idx + 3], memory_idx, memory, error, num_line, is_print);
                        break;
                    }
                }
                /* Handle error */
                if (*error != NO_ERROR) {
                    /* Handle error and reset error state */
                    handleError(error, num_line, is_print);
                    *error = NO_ERROR;
                    /* Move to the next code line and continue loop */
                    moveToNextCodeLine(&temp_code, &num_line);
                    continue; /* Skip the rest of the loop iteration */
                }
                /* Update IC and reset L value */
                *IC += L;
                L = DEFAULT_VALUE;
                break; /* End of case DOT_OTHER */
        }
        num_line++; /* update the line number */
        temp_code = temp_code->next; /* go to the next line */
    }

    /* Move data from data_memory to memory array */
    moveDataToMemory(data_memory, &data_memory_idx, memory, memory_idx, error);
    
    /* Check if there was no stop command */
    if (!stop_flag) {
        /* Set error and handle it */
        *error = ERROR_NO_STOP_COMMAND;
        handleError(error, num_line, is_print);
        *error = NO_ERROR; /* Reset error state */
    }
    
    /* Check if there's a memory allocation error or is_print is false */
    if (*error == ERROR_MAXED_OUT_MEMORY || is_print == false) {
        return; /* Return from the function */
    }
    
    /* Update memory addresses for labels */
    temp_label_node = *labels;
    while (temp_label_node) {
        switch (temp_label_node->label_type) {
            case LABEL_TYPE_DATA:
                temp_label_node->memory_adress += *IC;
            case LABEL_TYPE_CODE:
            case LABEL_TYPE_ENTRY:
                temp_label_node->memory_adress += DEFAULT_MEMORY_INDEX;
            /* Skip LABEL_TYPE_EXTERNAL and LABEL_TYPE_NOT_FOUND */
            case LABEL_TYPE_EXTERNAL:
            case LABEL_TYPE_NOT_FOUND:
                break;
        }
        temp_label_node = temp_label_node->next;
    }
    
    /* Free memory allocated for tokens */
    freeMemory(tokens, NULL, NULL, NULL);

}

/**
 * @brief Performs the second iteration of the assembler.
 * 
 * @param memory Global memory array.
 * @param memory_idx Index in the global memory array.
 * @param code Pointer to the code linked list.
 * @param labels Pointer to the labels linked list.
 * @param DC Data Counter.
 * @param IC Instruction counter.
 * @param error Pointer to an Error variable for error handling.
 * @param file_name The name of the output file.
 * @param externals Pointer to externals linked list.
 * @param is_print Pointer to a boolean indicating whether to create output files
 */
void secondIteration(short* memory, int* memory_idx, CodeNode* code, LabelNode* labels, int* DC, int* IC,
Error* error, char* file_name, LabelNode* externals, bool* is_print) {

    /* Initialize variables for the second iteration */
    CodeNode* temp_code;                    /* Pointer to traverse the code linked list */
    LabelNode* temp_label;                  /* Temporary pointer for labels linked list */
    bool stop_flag = false;                 /* Flag to indicate if a STOP command is encountered */
    bool is_first_itteration_flag = false;  /* Flag indicating the first iteration of the assembler */
    int token_idx = DEFAULT_VALUE;          /* Index to keep track of the current token being processed */
    bool label_flag = false;                /* Flag to indicate if the current line contains a label */
    char** tokens = allocateMemory(MAX_TOKENS * sizeof(char *), is_print, error); /* Array to store tokens */
    int num_tokens = DEFAULT_VALUE;         /* Number of tokens in the current code line */
    int L = DEFAULT_VALUE;                 /* Value of L for the current command */
    int num_line = STARTING_LINE;           /* Current line number being processed */
    int update_memory_idx = DEFAULT_MEMORY_INDEX; /* Index in the memory array for updating memory contents */
    int check_counter;                      /* Counter to track the number of memory words to update */
    short curr_memory;                      /* Value of memory at the current index */


/* Allocate memory for an array of tokens to hold the parsed code */
allocateMemoryTokens(tokens, is_print, error);

/* Initialize variables for code processing */
temp_code = code;                        /* Pointer to the code linked list */
*IC = DEFAULT_VALUE;                     /* Initialize the Instruction Counter (IC) */

/* Iterate through the code linked list */
while (temp_code) {
    token_idx = DEFAULT_VALUE;           /* Reset the token index for each line of code */
    label_flag = false;                 /* Initialize label flag for current line */

    /* Check if the line starts with a comment (';') */
    if (temp_code->code_row[FIRST_CHARACTER] == ';') {
        temp_code = temp_code->next;     /* Move to the next line of code */
        num_line++;                      /* Increment the line number */
        continue;                        /* Skip the rest of the loop for this line */
    }

    /* Check for empty lines or line termination characters ('\n', '\0', '\r') */
    if (temp_code->code_row[FIRST_CHARACTER] == '\n' || temp_code->code_row[FIRST_CHARACTER] == '\0' 
        || temp_code->code_row[FIRST_CHARACTER] == '\r') {
        temp_code = temp_code->next;     /* Move to the next line of code */
        num_line++;                      /* Increment the line number */
        continue;                        /* Skip the rest of the loop for this line */
    }

    /* Tokenize the current line of code into individual tokens */
    tokenizeInput(temp_code->code_row, tokens, &num_tokens, is_print, error);

    /* Handle memory allocation error */
    if (*error == ERROR_MEMORY_ALLOCATION) {
        handleError(error, num_line, is_print);
        return;                           /* Return from the function in case of error */
    }

    /* Check if the current line contains a label */
    if (isLabel(tokens[token_idx], true)) {
        label_flag = true;               /* Set the label flag to true */
        token_idx++;                     /* Move to the next token after the label */
    }
        /* Determine the type of dot command (e.g., .entry, .extern, .data, .string) */
        switch (getDotType(tokens[token_idx], error)) {
            case DOT_ENTRY:
                /* Check and process the .entry directive */
                checkExternalEntryLine(tokens, num_tokens, error, &labels, LABEL_TYPE_ENTRY, is_first_itteration_flag);

                /* Handle errors related to .entry directive */
                if (*error == ERROR_DUPLICATE_LABEL || *error == ERROR_NOT_ENOUGH_ARGUMENTS 
                || *error == ERROR_INCORRECT_OPERAND_TYPE 
                    || *error == ERROR_WRONG_NUM_OF_COMMAS || *error == ERROR_UNRECOGNIZED_LABEL)
                {
                    handleError(error, num_line, is_print);
                    *error = NO_ERROR; /* Reset the error status */
                    moveToNextCodeLine(&temp_code, &num_line); /* Move to the next code line */
                    continue; /*  Skip the rest of the loop for this line  and go to the next one */
                }
                
                /* Update entry labels with their corresponding memory addresses */
                updateEntryLabels(labels, tokens, num_tokens, token_idx);
                break;

                case DOT_OTHER:
                    /* Check the validity of the command line and process it */
                    if (checkCommandLine(tokens, num_tokens, label_flag, labels, error, is_first_itteration_flag, &stop_flag,
                                         is_print, num_line) != COMMAND_LINE_ERROR) {
                        /* Get the length of the command line */
                        L = checkCommandLine(tokens, num_tokens, label_flag, labels, error, is_first_itteration_flag, &stop_flag,
                                              is_print, num_line);

                        /* Initialize a counter to track memory updates */
                        check_counter = L;

                        /* Get the current memory content */
                        curr_memory = memory[update_memory_idx];

                        /* Update source operand */
                        if ((curr_memory & 0x600) == 0x600) {
                            /* Move to the next memory index */
                            update_memory_idx++;

                            /* Decrement the counter */
                            check_counter--;

                            /* Check for unresolved memory reference */
                            if (memory[update_memory_idx] == 0xFFF) {
                                temp_label = labels;

                                /* Search for the corresponding label */
                                while (temp_label) {
                                    if (!strcmp(tokens[token_idx + 1], temp_label->label_name)) {
                                        if (temp_label->label_type == LABEL_TYPE_EXTERNAL) {
                                            /* Set the memory value for an external label */
                                            memory[update_memory_idx] = 0x001;

                                            /* Insert the label into the externals list */
                                            insertNewLabel(&externals, temp_label->label_name, LABEL_TYPE_EXTERNAL, &update_memory_idx,
                                                           is_print, error, is_first_itteration_flag);

                                            /* Move to the next memory index */
                                            update_memory_idx++;

                                            /* Decrement the counter */
                                            check_counter--;
                                        } else {
                                            /* Set the memory value for a resolved label */
                                            memory[update_memory_idx] = temp_label->memory_adress;
                                            memory[update_memory_idx] <<= 2;
                                            memory[update_memory_idx] += 2;

                                            /* Move to the next memory index */
                                            update_memory_idx++;

                                            /* Decrement the counter */
                                            check_counter--;
                                        }
                                        break;
                                    }
                                    temp_label = temp_label->next;
                                }
                            }
                        } else {
                            /* Handle memory updates for specific cases */
                            if ((curr_memory & 0xE00) != 0x000) {
                                /* Move to the next memory index */
                                update_memory_idx++;

                                /* Decrement the counter */
                                check_counter--;
                            }
                        }

                        /* Update token index based on memory configuration */
                        if ((curr_memory & 0xE00) != 0x000) {
                            /* Move token index to skip operands */
                            token_idx += 3;
                        } else {
                            /* Move token index to the next operand */
                            token_idx++;
                        }

                        /* Update destination operand */
                        if ((curr_memory & 0x00C) == 0x00C) {
                            /* Move to the next memory index */
                            update_memory_idx++;

                            /* Decrement the counter */
                            check_counter--;

                            /* Check for unresolved memory reference */
                            if (memory[update_memory_idx] == 0xFFF) {
                                temp_label = labels;

                                /* Search for the corresponding label */
                                while (temp_label) {
                                    if (!strcmp(tokens[token_idx], temp_label->label_name)) {
                                        if (temp_label->label_type == LABEL_TYPE_EXTERNAL) {
                                            /* Set the memory value for an external label */
                                            memory[update_memory_idx] = 0x001;

                                            /* Insert the label into the externals list */
                                            insertNewLabel(&externals, temp_label->label_name, LABEL_TYPE_EXTERNAL, &update_memory_idx,
                                                           is_print, error, is_first_itteration_flag);

                                            /* Move to the next memory index */
                                            update_memory_idx++;

                                            /* Decrement the counter */
                                            check_counter--;
                                        } else {
                                            /* Set the memory value for a resolved label */
                                            memory[update_memory_idx] = temp_label->memory_adress;
                                            memory[update_memory_idx] <<= 2;
                                            memory[update_memory_idx] += 2;

                                            /* Move to the next memory index */
                                            update_memory_idx++;

                                            /* Decrement the counter */
                                            check_counter--;
                                        }
                                        break;
                                    }
                                    temp_label = temp_label->next;
                                }
                            }
                        }

                        /* Update the memory index to account for the checks and updates */
                        update_memory_idx += check_counter;
                    }

                    /* Handle any errors encountered during processing */
                    if (*error != NO_ERROR) {
                        handleError(error, num_line, is_print);
                        *error = NO_ERROR;
                        moveToNextCodeLine(&temp_code, &num_line);
                        continue;
                    }

                    /* Increment the instruction counter */
                    *IC += L;
                    break;
            
                    
                    
                    
            case DOT_EXTERN:
                /* Check and process external label declaration */
                checkExternalEntryLine(tokens, num_tokens, error, &labels, LABEL_TYPE_EXTERNAL, is_first_itteration_flag);
                /* Handle errors related to external label declaration */
                if (*error == ERROR_DUPLICATE_LABEL || *error == ERROR_NOT_ENOUGH_ARGUMENTS || *error == ERROR_INCORRECT_OPERAND_TYPE
                    || *error == ERROR_WRONG_NUM_OF_COMMAS)
                {
                    /* Handle the error and continue processing */
                    handleError(error, num_line, is_print);
                    *error = NO_ERROR;
                    moveToNextCodeLine(&temp_code, &num_line);
                    continue;
                }
                break;
            case DOT_DATA:
            case DOT_STRING:
                /* Handle .data and .string commands; no processing needed */
                break;
        }

        /* Move to the next line and code node */
        num_line++;
        temp_code = temp_code->next;

        /* Reset the length counter for the next line */
        L = DEFAULT_VALUE;
    }
    
    /* Check for a file handle error and handle it if necessary */
    if (*error == ERROR_FILE_HANDLE){
        handleError(error, num_line, is_print);
        return;
    }

    /* Check if the stop flag is not set */
    if (!stop_flag)
    {
        /* Set the appropriate error, handle it, and reset the error */
        *error = ERROR_NO_STOP_COMMAND;
        handleError(error, num_line, is_print);
        *error = NO_ERROR;
    }

    /* Create output files based on the assembled program */
    createOutputFiles(file_name, labels, memory, memory_idx, *IC, *DC, externals, is_print, error, num_line);

    /* Free memory used for tokens, code nodes, and labels */
    freeMemory(tokens, code, NULL, labels);

    /* Free memory used for the externals label linked list */
    freeMemoryLabelNode(externals);

}

/**
 * @brief Creates the binary word for an operand based on its type.
 * 
 * @param L - The L value (number of memory words) for the operand.
 * @param labels - Pointer to the labels linked list.
 * @param is_first_iteration - Flag indicating the first iteration.
 * @param op_type_1 - The source operand type.
 * @param op_type_2 - The destination operand type.
 * @param operand1 - The source operand value.
 * @param operand2 - The destination operand value.
 * @param memory_idx - Index in the global memory array.
 * @param memory - Global memory array.
 * @param error - Pointer to an Error variable for error handling.
 * @param num_line - The current line number.
 * @param is_print - Pointer to a boolean indicating whether to create output files.
 */
void createOperandBinaryWord(int L, LabelNode* labels, bool is_first_iteration, OperandType op_type_1,
OperandType op_type_2, char* operand1, char* operand2, int* memory_idx, short* memory, Error* error,
int num_line, bool* is_print) {
    short resulting_binary_word = 0x0;

    switch (L) {
        case 1:
            /* For L=1, the operand doesn't require extra binary words. No action needed. */
            break;
        case 2:
            if (op_type_1 == OPERAND_TYPE_REGISTER && op_type_2 == OPERAND_TYPE_REGISTER) {
                /* Two registers case: Convert register numbers to binary and assemble the binary word */
                resulting_binary_word += (short) atoi(operand1 + 2); /* Convert register number and shift it to the left */
                resulting_binary_word <<= 5;
                resulting_binary_word += (short) atoi(operand2 + 2); /* Convert register number and shift it to the left */
                resulting_binary_word <<= 2;
                /* Push the assembled binary word to the memory array */
                pushToMemory(memory_idx, memory, resulting_binary_word, error, num_line, is_print);
                if (*error == ERROR_MAXED_OUT_MEMORY) return;
            } else {
                /* Single operand case: Call createBinaryWordByType to handle the operand */
                createBinaryWordByType(labels, op_type_1, operand1, memory, memory_idx, is_first_iteration, error,
                num_line, is_print);
            }
            break;
        case 3: 
            /* Two operand case: Create binary words for both operands */
            createBinaryWordByType(labels, op_type_1, operand1, memory, memory_idx, is_first_iteration, error,
            num_line, is_print);
            createBinaryWordByType(labels, op_type_2, operand2, memory, memory_idx, is_first_iteration, error,
            num_line, is_print);
            break;
    }
}



/**
 * @brief Creates a binary word based on the operand type and stores it in memory.
 * 
 * @param labels - Pointer to the labels linked list.
 * @param op_type - The type of operand.
 * @param operand - The operand value.
 * @param memory - Global memory array.
 * @param memory_idx - Index in the global memory array.
 * @param is_first_iteration - Flag indicating the first iteration.
 * @param error - Pointer to an Error variable for error handling.
 * @param num_line - The current line number.
 * @param is_print - Pointer to a boolean indicating whether to create output files.
 */
void createBinaryWordByType(LabelNode* labels, OperandType op_type, char* operand, short* memory, int* memory_idx,
bool is_first_iteration, Error* error, int num_line, bool* is_print) {
    short resulting_binary_word = 0x0;
    LabelNode* temp_label_node;

    switch (op_type) {
        case OPERAND_TYPE_REGISTER:
            resulting_binary_word += (short) atoi(operand + 2); /* Convert register number and shift it to the left */
            resulting_binary_word <<= 7;
            pushToMemory(memory_idx, memory, resulting_binary_word, error, num_line, is_print);
            if (*error == ERROR_MAXED_OUT_MEMORY) return;
            break;
        case OPERAND_TYPE_LABEL:
            if (!is_first_iteration) {
                temp_label_node = labels;
                while (temp_label_node) {
                    if (!strcmp(temp_label_node->label_name, operand)) {
                        resulting_binary_word += temp_label_node->memory_adress; /* Get label address and shift it to the left */
                        resulting_binary_word <<= 2;
                        pushToMemory(memory_idx, memory, resulting_binary_word, error, num_line, is_print);
                        if (*error == ERROR_MAXED_OUT_MEMORY) return;
                    }
                    temp_label_node = temp_label_node->next;
                }
            } else {
                pushToMemory(memory_idx, memory, 0xFFF, error, num_line, is_print); /* Fill with ones during first iteration */
                if (*error == ERROR_MAXED_OUT_MEMORY) return;
            }
            break;
        case OPERAND_TYPE_NUMBER:
            resulting_binary_word += (short) atoi(operand); /* Convert number and shift it to the left */
            resulting_binary_word <<= 2;
            pushToMemory(memory_idx, memory, resulting_binary_word, error, num_line, is_print);
            if (*error == ERROR_MAXED_OUT_MEMORY) return;
            break;
        case OPERAND_TYPE_OTHER:
            break;
    }
}


/**
 * @brief Creates the binary word for a command and its operands.
 * 
 * @param tokens - An array of tokens.
 * @param num_tokens - The number of tokens.
 * @param token_idx - The index of the current token.
 * @param error - Pointer to an Error variable for error handling.
 * @param is_first_itteration - Flag indicating the first iteration.
 * @param labelPtr - Pointer to the labels linked list.
 * @return short - The generated binary word.
 */
short createCommandBinaryWord(char** tokens, int num_tokens, int token_idx, Error* error,
bool is_first_itteration, LabelNode* labelPtr) {
    short resulting_binary_word = DEFAULT_VALUE;
    short source_operand, destination_operand;
    int temp_idx = token_idx;
    int operand_amount;
    int opcode;

    operand_amount = getOperandAmount(tokens[temp_idx]); /* Get the number of operands for the command */
    opcode = checkCommand(tokens[temp_idx++]); /* Get the opcode for the command */

    switch (operand_amount) {
        case 0:
            source_operand = destination_operand = 0x0;
            break;
        case 1:
            /* Get addressing method for the destination operand */
            destination_operand = getAdressingMethodByOperandType(checkOperand(tokens[temp_idx++], labelPtr, error,
            is_first_itteration));
            source_operand = 0x0;
            break;
        case 2:
            /* Get addressing method for source and destination operands */
            source_operand = getAdressingMethodByOperandType(checkOperand(tokens[temp_idx++], labelPtr, error,
            is_first_itteration));
            destination_operand = getAdressingMethodByOperandType(checkOperand(tokens[++temp_idx], labelPtr, error,
            is_first_itteration));
            break;
    }
    /* Construct the resulting binary word by placing the values of source_operand, opcode, and destination_operand */
    resulting_binary_word +=  (source_operand << 9);
    resulting_binary_word +=  (opcode << 5);
    resulting_binary_word +=  (destination_operand << 2);
    resulting_binary_word +=  0; /* Fill the last two bits with zeros */
    return resulting_binary_word;
}


/**
 * @brief Gets the number of operands for a command based on its opcode.
 * 
 * @param opcode - The opcode of the command.
 * @return int - The number of operands.
 */
int getOperandsNumberByOpcode(short opcode) {
    int i;

    for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
        if (commands[i].opcode == opcode) {
            return commands[i].number_of_operands; /* Return the number of operands for the given opcode */
        }
    }
    return DEFAULT_ERROR_VALUE; /* Return an error value if the opcode is not recognized */
}


/**
 * @brief Gets the addressing method value based on the operand type.
 * 
 * @param operand_type - The type of operand.
 * @return int - The addressing method value.
 */
int getAdressingMethodByOperandType(OperandType operand_type) {
    switch (operand_type) {
        case OPERAND_TYPE_NUMBER:   return ADRRESING_METHOD_NUMBER; /* Return the addressing method value for a number operand */
        case OPERAND_TYPE_LABEL:    return ADRRESING_METHOD_LABEL;  /* Return the addressing method value for a label operand */
        case OPERAND_TYPE_REGISTER: return ADRRESING_METHOD_REGISTER; /* Return the addressing method value for a register operand */
        case OPERAND_TYPE_OTHER:    return ADRRESING_METHOD_OTHER; /* Return the addressing method value for other operand types */
    }
    return DEFAULT_ERROR_VALUE; /* Return an error value if the operand type is not recognized */
}


/**
 * @brief Gets the number of operands for a command based on its mnemonic.
 * 
 * @param command - The mnemonic of the command.
 * @return int - The number of operands.
 */
int getOperandAmount(char* command) {
    int i;

    /* Loop through the commands array to find the matching command mnemonic */
    for (i = 0; i < NUMBER_OF_COMMANDS; i++) {
        if (!strcmp(command, commands[i].command)) {
            return commands[i].number_of_operands; /* Return the number of operands for the command */
        }
    }
    return DEFAULT_ERROR_VALUE; /* Return an error value if the command is not recognized */
}


/**
 * @brief Updates entry labels with their corresponding memory addresses.
 * 
 * @param labels - Pointer to the labels linked list.
 * @param tokens - An array of tokens.
 * @param num_tokens - The number of tokens.
 * @param token_idx - The index of the token.
 */
void updateEntryLabels(LabelNode* labels, char** tokens, int num_tokens, int token_idx) {
    LabelNode* temp_label;
    
    /* Loop through the tokens starting from the specified token index */
    for (; token_idx < num_tokens; token_idx++) {
        if (isLabel(tokens[token_idx], false)) { /* Check if the token is a label */
            temp_label = labels;

            /* Search for the label in the labels linked list and update its type */
            while (temp_label) {
                if (!strcmp(temp_label->label_name, tokens[token_idx])) {
                    temp_label->label_type = LABEL_TYPE_ENTRY;
                }
                temp_label = temp_label->next;
            }
        }
    }
}


/**
 * @brief Create an Output Files object.
 * 
 * @param file_name - Takes the assembly code file name.
 * @param labels - Pointer to the labels linked list.
 * @param memory - Pointer to the global memory array.
 * @param memory_idx - Index in the global memory array, points to the latest free memory slot.
 * @param IC - Instruction counter.
 * @param DC - Data Counter.
 * @param externals - Pointer to externals linked list.
 * @param is_print - Pointer to a boolean indicating whether to create output files.
 * @param error - Pointer to error, for handling errors.
 * @param num_line - The line number on which the function is performed (part of the error handling mechanism).
 */
void createOutputFiles(char* file_name, LabelNode* labels, short* memory, int* memory_idx, int IC,
int DC, LabelNode* externals, bool* is_print, Error* error, int num_line) {
    if (*is_print) { /* Check if output file creation is enabled */

        /* Create output files for entry labels, externals, and memory dump */
        createFileWithLabelType(file_name, labels, LABEL_TYPE_ENTRY, is_print, error);
        createFileWithLabelType(file_name, externals, LABEL_TYPE_EXTERNAL, is_print, error);
        createFileWithMemoryDump(file_name, memory, memory_idx, IC, DC, error, num_line, is_print);
    }
}


/**
 * @brief Creates a memory dump file with the contents of the memory array.
 * 
 * @param file_name - The name of the output file.
 * @param memory - Pointer to the global memory array.
 * @param memory_idx - Index in the global memory array.
 * @param IC - Instruction counter.
 * @param DC - Data counter.
 * @param error - Pointer to an Error variable for error handling.
 * @param num_line - The current line number.
 * @param is_print - Indicates whether to create output files.
 */
void createFileWithMemoryDump(char* file_name, short* memory, int* memory_idx, int IC, int DC, Error* error,
int num_line, bool* is_print) {
    FILE *fptr; /* File pointer for writing output */
    int i; /* Loop index */
    char output_file_name[MAX_FILE_NAME_WITH_EXTENSION]; /* Holds the name of the output file */
    char base64[3]; /* Temporary buffer for base64 conversion */

    cleanLine(output_file_name, MAX_FILE_NAME_WITH_EXTENSION);
    strcat(output_file_name, "output/");
    strcat(output_file_name, file_name);
    strcat(output_file_name, ".ob");

    /* Open the output file for writing */
    fptr = fopen(output_file_name, "w");

    /* Check if the file was opened successfully */
    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error, num_line, is_print);
        return;
    }

    /* Write the values of IC and DC to the file */
    fprintf(fptr, "%d %d\n", IC, DC);

    /* Adjust the memory index if DC is 0 */
    if (DC == 0) {
        (*memory_idx)--;
    }

    /* Loop through the memory array and write values to the file */
    for (i = DEFAULT_MEMORY_INDEX; i < *memory_idx; i++) {
        convertToBase64(memory[i], base64); /* Convert memory value to base64 */
        fprintf(fptr, "%s\n", base64);
    }

    /* Close the output file */
    fclose(fptr);
}


/**
 * @brief Creates an output file with the specified label type (entry or extern).
 * 
 * @param file_name - The name of the output file.
 * @param labels - Pointer to the labels linked list.
 * @param label_type - The type of label (entry or extern).
 * @param is_print - Indicates whether to create output files.
 * @param error - Pointer to an Error variable for error handling.
 */
void createFileWithLabelType(char* file_name, LabelNode* labels, LabelType label_type, bool* is_print, Error* error) {
    FILE *fptr; /* File pointer for writing output */
    bool is_entry = false; /* Flag to check if entry label is present */
    LabelNode* temp = labels; /* Temporary label node for traversing the list */
    char output_file_name[MAX_FILE_NAME_WITH_EXTENSION]; /* Holds the name of the output file */
    
    cleanLine(output_file_name, MAX_FILE_NAME_WITH_EXTENSION);

    /* Construct the output file name based on the label type */
    strcat(output_file_name, "output/");
    strcat(output_file_name, file_name);
    switch (label_type) {
        case LABEL_TYPE_ENTRY:
            strcat(output_file_name, ".ent");

            /* Check if there is an entry label */
            while (temp) {
                if (temp->label_type == LABEL_TYPE_ENTRY) {
                    is_entry = true;
                    break;
                }
                temp = temp->next;
            }
            if (!is_entry) {
                return; /* No entry labels, no need to create the file */
            }
            break;
        case LABEL_TYPE_EXTERNAL:
            strcat(output_file_name, ".ext");
            if (!labels) {
                return; /* No external labels, no need to create the file */
            }
            break;
        default:
            *error = ERROR_UNRECOGNIZED_LABEL;
            return; /* Unrecognized label type */
    }
    
    /* Open the output file for writing */
    fptr = fopen(output_file_name, "w");

    /* Check if the file was opened successfully */
    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error, DEFAULT_LINE_NUMER, is_print);
        return;
    }

    /* Write the labels into the corresponding output file type */
    while (labels) {
        if (labels->label_type == label_type) {
            fprintf(fptr, "%s %d\n", labels->label_name, labels->memory_adress);
        }
        labels = labels->next;
    }
    
    /* Close the output file */
    fclose(fptr);
}


/**
 * @brief Creates the file with the code after pasting macros
 * 
 * @param file_name - Contains the name of the input file
 * @param code - Pointer to the head of the code linked list
 * @param is_print - Indicates whether to create output files
 * @param error - Pointer to an Error variable for error handling
 */
void createCodeFileWithoutMacros(char* file_name, CodeNode* code, bool* is_print, Error* error) {
    char output_file_name[MAX_FILE_NAME_WITH_EXTENSION]; /* Holds the name of the output file */
    FILE *fptr; /* File pointer for writing output */
    
    cleanLine(output_file_name, MAX_FILE_NAME_WITH_EXTENSION);

    /* Construct the output file name */
    strcat(output_file_name, "output/");
    strcat(output_file_name, file_name);
    strcat(output_file_name, ".am");

    /* Open the output file for writing */
    fptr = fopen(output_file_name, "w");

    /* Check if the file was opened successfully */
    if (!fptr) {
        *error = ERROR_FILE_HANDLE;
        handleError(error, DEFAULT_LINE_NUMER, is_print);
        return;
    }

    /* Write the code rows to the output file */
    while (code) {
        fprintf(fptr, "%s\n", code->code_row);
        code = code->next;
    }
    
    /* Close the output file */
    fclose(fptr);
}


/**
 * @brief checks if the string is a label
 * 
 * @param word - string that should be checked
 * @param colon - if the end of label has to have ":" - put true, otherwise false
 * @return boolean that indicates whether the word is a legal label name
 */
bool isLabel(char* word, bool colon){
    int i = 0; /* index used for going throught the word */

    if (!isalpha(word[i++])) {
        return false;
    }
    for (; word[i] != '\0'; i++) {
        if (colon && word[i] == ':' && word[i+1] == '\0') {
            return true;
        }
        if (!colon && word[i] == ':') {
            return false;
        }
        if (!isalpha(word[i]) && !isdigit(word[i])) {
            return false;
        }
    }
    return !colon;
}

/**
 * @brief Determines the type of a dot command (e.g., .data, .string, .entry, .extern).
 * 
 * @param word The dot command string.
 * @param error Pointer to an Error variable for error handling.
 * @return The corresponding DotType value:
 *         - DOT_EXTERN if it's ".extern"
 *         - DOT_ENTRY if it's ".entry"
 *         - DOT_STRING if it's ".string"
 *         - DOT_DATA if it's ".data"
 *         - DOT_OTHER if it's none of the above.
 */
DotType getDotType(char* word, Error* error){
    if (!strcmp(word, ".data")) return DOT_DATA;
    if (!strcmp(word, ".string")) return DOT_STRING;
    if (!strcmp(word, ".entry")) return DOT_ENTRY;
    if (!strcmp(word, ".extern")) return DOT_EXTERN;
    return DOT_OTHER;
}

/**
 * @brief Determines the type of a label (external, entry, code) based on its name.
 * 
 * @param label The label name to be checked.
 * @param labels Pointer to the head of the LabelNode linked list.
 * @param error Pointer to an Error variable for error handling.
 * @return The corresponding LabelType value:
 *         - LABEL_EXTERNAL if it's an external label
 *         - LABEL_ENTRY if it's an entry label
 *         - LABEL_CODE if it's a regular label
 *         - LABEL_OTHER if it's none of the above.
 */
LabelType getLabelType(char* label, LabelNode* LabelPtr, Error* error){
    while (LabelPtr) {
        if (!strcmp(label, LabelPtr->label_name)) {
            return LabelPtr->label_type;
        }
        LabelPtr = LabelPtr->next;
    }
    *error = ERROR_UNRECOGNIZED_LABEL;
    return LABEL_TYPE_NOT_FOUND;
}

/**
 * @brief Checks if a line is a correct data line or not
 * 
 * @param tokens - Array of tokens in the line
 * @param num_tokens - Number of tokens in the line
 * @param label - Boolean indicating if the first token is a label
 * @param error - Pointer to an Error enum that indicates any errors that occur
 * @return true - If it is a correct data line
 * @return false - When it's not a data line or an incorrect one
 */
bool checkDataLine(char** tokens, int num_tokens, bool label, Error* error){
    int token_index = 1; 

    /* Check for missing data argument */
    if (num_tokens < (2 + label)) {
        *error = ERROR_MISSING_DATA_ARGUMENT;
        return false;
    }

    /* checks .string line */
    if (getDotType(tokens[FIRST_WORD + label], error) == DOT_STRING) {
        if (isString(tokens, num_tokens, label)) {
            return true;
        }
        /* the argument to .string is wrong */
        else {
            *error = ERROR_WRONG_ARGUMENT_FORMAT;
            return false;
        }
    }
    
    /* check .data line */
    if (getDotType(tokens[FIRST_WORD + label], error) == DOT_DATA) {
        if (num_tokens % 2 == (!label)) {
            *error = ERROR_WRONG_NUM_OF_COMMAS;
            return false;
        }
        token_index += label;

        /* loops throught all the arguments to .data */
        for (; token_index< num_tokens; token_index+=2) {
            if (!isNumber(tokens[token_index])) {
                *error = ERROR_WRONG_ARGUMENT_FORMAT;
                return false;
            }
        }

        /* loops throught all the places in the line , and checks if there is a comma
         where its needed to be */
        for (; token_index + 1 < num_tokens; token_index += 2) {
            if (strcmp(tokens[token_index], ",")) {
                *error = ERROR_MISSING_COMMA;
                return false;
            }
        }
        return true;
    }
    return false;
}

/**
 * @brief Pushes a memory field value into memory.
 * 
 * @param memory_idx Index in the global memory array.
 * @param memory Global memory array.
 * @param memoryField The memory field value to push.
 * @param error Pointer to an Error variable for error handling.
 * @param num_line The current line number.
 * @param is_print Pointer to a boolean indicating whether to create output files
 */
void pushToMemory(int* memory_idx, short* memory, short memoryField, Error* error, int num_line, bool *is_print) {
    if (*memory_idx >= MAX_MEMORY_SIZE) {
        *error = ERROR_MAXED_OUT_MEMORY;
        handleError(error, num_line, is_print);
        return;
    }
    memory[(*memory_idx)++] = memoryField;
}

/**
 * @brief Inserts a new label node into the linked list of labels
 * 
 * @param labels - Pointer to the head of the label linked list
 * @param label_name - Name of the label to be inserted
 * @param label_type - Type of the label (DATA, CODE, etc.)
 * @param memory_idx - Pointer to the memory index in the global memory array.
 * @param is_print - Pointer to a boolean flag indicating whether to create output files
 * @param error - Pointer to an Error enum that indicates for error handling
 * @param is_first_iteration_flag - Flag indicating if it's the first iteration 
 */
void insertNewLabel(LabelNode** labels, char* label_name, LabelType label_type, int* memory_idx, bool* is_print, Error* error, bool is_first_itteration_flag) {
    LabelNode* temp_label; /* Temporary pointer for traversing the linked list */
    LabelNode* new_label; /* New label node to be inserted */
    temp_label = *labels; /* Initialize temp_label to the head of the linked list */

    /* Check for duplicated label and handle it */
    if (isDuplicatedLabel(labels, label_name, label_type, error, is_first_itteration_flag))
    {
        return;
    }
    
    *error = NO_ERROR;
    /* if the head node isn't null */
    if (temp_label) {

        /* loop through linked list, untill getting to the end */
        while(temp_label && temp_label->next) {
            temp_label = temp_label->next;
        }
        /* allocating memory to the new node, in which the label name will be stored */
        new_label = (LabelNode*) allocateMemory(sizeof(LabelNode), is_print, error);
        new_label->label_name = allocateMemory(sizeof(label_name) * sizeof(char), is_print, error);

        /* add the label  into the linked list */        
        strcpy(new_label->label_name, label_name);
        new_label->label_type = label_type;
        new_label->memory_adress = *memory_idx;
        temp_label->next = new_label;
    } else {
        /* creates the first label node in the linked list */
        (*labels) = (LabelNode*) allocateMemory(sizeof(LabelNode), is_print, error);
        (*labels)->label_name = allocateMemory(sizeof(label_name) * sizeof(char), is_print, error);
        strcpy((*labels)->label_name, label_name);
        (*labels)->label_type = label_type;
        (*labels)->memory_adress = *memory_idx;
    }
}

/**
 * @brief Takes a line of code with a command and checks if it's legal
 * 
 * @param word - The command word to be checked
 * 
 * @return short - Returns the opcode of the command if found, otherwise DEFAULT_ERROR_VALUE
 */
short checkCommand(char* word) {
    int i; /* Index used in for loop */

    /* Iterate through the list of commands to find a match */
    for (i = 0; i < NUM_OF_COMMANDS; i++) {
        if (!strcmp((char*)commands[i].command, word)) {
            return commands[i].opcode; /* Return the opcode if the command is found */
        }
    }
    return DEFAULT_ERROR_VALUE; /* Return DEFAULT_ERROR_VALUE if the command is not found */
}


/**
 * @brief Takes a line of code with a command and checks if it's legal
 * 
 * @param tokens - Array of tokenized strings from the line of code
 * @param num_tokens - Number of tokens in the array
 * @param label - Indicates if the line starts with a label
 * @param LabelPtr - Pointer to a linked list of label nodes
 * @param error - Pointer to the error flag
 * @param is_first_iteration - Flag indicating if it's the first iteration
 * @param stop_flag - Flag indicating if a stop command is encountered
 * @param is_print - Flag indicating if printing is enabled
 * @param num_line - Current line number
 * 
 * @return int - Returns L = the number of memory words in the command line, -1 otherwise
 */
int checkCommandLine(char** tokens, int num_tokens, bool label, LabelNode* LabelPtr, Error* error,
bool is_first_iteration, bool* stop_flag, bool* is_print, int num_line) {
    short opcode = checkCommand(tokens[label]);

    int count = DEFAULT_VALUE; /* used as help for counting the index of operands */
    int operand_index = label+1; /* operand index */
    OperandType operand_result; /* stores the operand type */
    bool register_flag = false; /* flag that checks if there was an operand argument , that is a register (used for knowing if there are 2 register type operands ) */
    bool source_flag = true; /* flag that looks after the operand if its a source or destination*/
    int L = 1;

    if (opcode == COMMAND_STOP)
    {
        *stop_flag = true;
    }

    /*ERROR unrecognized command name*/
    if (opcode == DEFAULT_ERROR_VALUE) {
        *error = ERROR_UNDEFINED_COMMAND; 
        return COMMAND_LINE_ERROR;
    }

    /* ERROR wrong amount of operands for the command */
    if ((num_tokens <= 2 + label) && ((num_tokens - label - 1) != commands[opcode].number_of_operands)) {
        *error = ERROR_WRONG_AMOUNT_OF_OPERANDS;
        return COMMAND_LINE_ERROR;
    }
    
    /*ERROR illegal comma*/
    if (!strcmp(tokens[num_tokens-1], ",")) {
        *error = ERROR_ILLEGAL_COMMA;
        return COMMAND_LINE_ERROR;
    }
    
    /* checks the number of operands for command that requires 2 operands*/
    if ((num_tokens > 2 + label) && (num_tokens - label - 2) != commands[opcode].number_of_operands) {
        *error = ERROR_WRONG_AMOUNT_OF_OPERANDS;
        return COMMAND_LINE_ERROR;
    }
    
    /*  loop that goes through arguments if there are any, and checks them. Otherwise skip the loop   */
    for (; count < commands[opcode].number_of_operands; count++) {
        if (!source_flag) {
            operand_result = checkOperand(tokens[operand_index + count + 1], LabelPtr, error, is_first_iteration);
        }
        else {           
            operand_result = checkOperand(tokens[operand_index + count], LabelPtr, error, is_first_iteration);
        }
        switch (operand_result) {
            case OPERAND_TYPE_LABEL:
                L++;

                /* checks commands that require less then 2 operands, for correctness of arguments type */
                if (commands[opcode].number_of_operands < 2) {
                    if (!commands[opcode].destinationAddresingMethod[ADDRESING_LABEL]) {
                        *error = ERROR_INCORRECT_OPERAND_TYPE;
                        return COMMAND_LINE_ERROR;
                    }
                    break;
                }

                /* checks the commands that require 2 operands, for correctness of arguments type*/
                if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_LABEL])
                || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_LABEL])) {
                    *error = ERROR_INCORRECT_OPERAND_TYPE;
                    return COMMAND_LINE_ERROR;
                }
                break;
            case OPERAND_TYPE_REGISTER:

                /* checks commands that require less then 2 operands, for correctness of arguments type*/
                if (commands[opcode].number_of_operands < 2) {
                    if (!commands[opcode].destinationAddresingMethod[ADDRESING_REGISTER]) {
                        *error = ERROR_INCORRECT_OPERAND_TYPE;
                        return COMMAND_LINE_ERROR;
                    }
                    else {
                        L++;
                        break;
                    }
                }
                /* checks the commands that require 2 operands, for correctness of arguments type*/
                if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_REGISTER])
                || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_REGISTER])) {
                    *error = ERROR_INCORRECT_OPERAND_TYPE;
                    return COMMAND_LINE_ERROR; 
                }

                /* No matter the amount of registers, give only 1 memmory word for it 
                    (gives only +1 to L, even if there are 2 registers as arguments for a command)
                */
                if (!register_flag) {
                    L++;
                    register_flag = true;
                }
                break;
            case OPERAND_TYPE_NUMBER:
                /*checks commands that require less then 2 operands, for correctness of arguments type*/
                if (commands[opcode].number_of_operands < 2) {
                    if (!commands[opcode].destinationAddresingMethod[ADDRESING_NUMBER]) {
                        *error = ERROR_INCORRECT_OPERAND_TYPE;
                        return COMMAND_LINE_ERROR;
                    }
                    else {
                        L++;
                        break;
                    }
                }
                
                /* checks the commands that require 2 operands, for correctness of arguments type*/
                if ((source_flag && !commands[opcode].sourceAddresingMethod[ADDRESING_NUMBER])
                || (!source_flag && !commands[opcode].destinationAddresingMethod[ADDRESING_NUMBER])) {
                    *error = ERROR_INCORRECT_OPERAND_TYPE;
                    return COMMAND_LINE_ERROR; 
                }
                L++;
                break;
            case OPERAND_TYPE_OTHER:
                *error = ERROR_ILLEGAL_OPERAND_TYPE;
                handleError(error, num_line, is_print);
                *error = NO_ERROR;
                return COMMAND_LINE_ERROR;
            }
        if (source_flag) source_flag = false;

        operand_result = DEFAULT_ERROR_VALUE;
    }
    return L;
}

/**
 * @brief Returns type of the operand that it recieves
 * 
 * @param operand - string that contains the operand that should be checked
 * @param LabelPtr - Pointer to the list of labels
 * @param error pointer to error, for handlaing errors
 * @param is_first_iteration - flag if the function is called out of the first iteration
 * @return boolean that indicates whether the word is a legal label name
 */
OperandType checkOperand(char* operand, LabelNode* LabelPtr, Error* error, bool is_first_iteration){    
    const char* registers[] = {"@r0", "@r1", "@r2", "@r3", "@r4", "@r5", "@r6", "@r7"}; /* declaring an array with registers as strings */
    int i; /* index that is used for loops */

    /* Check if the operand is one of the registers */
    for (i = 0; i < NUM_OF_REGISTERS; i++) {
        if (!strcmp(registers[i], operand)) {
            return OPERAND_TYPE_REGISTER;
        }
    }

    /* Check if the operand is a number */
    if (isNumber(operand)) {
        return OPERAND_TYPE_NUMBER;
    }

    /* Check if the operand is a legal label, depending on the iteration */
    if (is_first_iteration) {
        if (isLabel(operand, false)) {
            return OPERAND_TYPE_LABEL;
        }
    } else if (getLabelType(operand, LabelPtr, error) != LABEL_TYPE_NOT_FOUND) {
        return OPERAND_TYPE_LABEL;
    }

    /* Handle error case */
    return OPERAND_TYPE_OTHER;
}

/**
 * @brief moves data from data_memory array , to global memmory.
 * 
 * @param data_memory  the place where data information is stored , points to the latest free memory slot
 * @param data_memory_idx index in the data array memory
 * @param memory  global memory array
 * @param memory_idx index in the global  memory array , points to the latest free memory slot
 * @param error pointer to error, for handlaing errors
 */
void moveDataToMemory(short* data_memory, int* data_memory_idx, short* memory, int* memory_idx, Error* error){
    /* Set the initial index for data_memory */
    *data_memory_idx = DEFAULT_MEMORY_INDEX;

    /* Loop until either data_memory or memory is maxed out */
    while (*data_memory_idx < MAX_MEMORY_SIZE && *memory_idx < MAX_MEMORY_SIZE) {

        /* Copy data from data_memory to memory */
        memory[*memory_idx] = data_memory[*data_memory_idx];
        (*memory_idx)++;
        (*data_memory_idx)++;

        /* Check for termination conditions */
        if (*data_memory_idx == MAX_MEMORY_SIZE || data_memory[*data_memory_idx] == DEFAULT_ERROR_VALUE) {
            break;
        }
    }

    /* Check if memory limits were reached */
    if (*data_memory_idx >= MAX_MEMORY_SIZE || *memory_idx >= MAX_MEMORY_SIZE) {
        *error = ERROR_MAXED_OUT_MEMORY;
        return;
    }
}

/**
 * @brief Moves pointer to the next code line and increments num_line counter by 1
 * 
 * @param temp_code Pointer to the current code node
 * @param num_line Pointer to the current number of line
 */
void moveToNextCodeLine(CodeNode** temp_code, int* num_line){
    /* Move to the next CodeNode and increment the line number */
    *temp_code =  (*temp_code)->next;
    (*num_line)++;
}

/**
 * @brief checks if a line is a correct .extern or .entry line or not
 * 
 * @param tokens - array of tokens
 * @param num_tokens - number of tokens
 * @param error - Pointer to an Error variable for error handling.
 * @param labels - Array of labels
 * @param label_type - Type of the 
 * @param is_first_iteration - true if the function called from the first iteration
 * @return true - if it is a correct line
 * @return false - if it is a incorrect line
 */
bool checkExternalEntryLine(char** tokens, int num_tokens, Error* error, LabelNode** labels, LabelType label_type, bool is_first_itteration){
    int operand_index = FIRST_ARGUMENT; /* index of the operand */
    bool entryLine = false; /* flag that says if the line is an .entry line */

    if (label_type == LABEL_TYPE_ENTRY) {
        entryLine = true;
    }
    /* Check if there are enough tokens */
    if (num_tokens < 2) {
        *error = ERROR_NOT_ENOUGH_ARGUMENTS;
        return false;
    }

    /* Check if the number of tokens is even */
    if (num_tokens % 2 != EVEN) {
        *error = ERROR_WRONG_NUM_OF_COMMAS;
        return false;
    }

    /* Check for missing commas */
    for (; operand_index + FIRST_ARGUMENT < num_tokens; operand_index += 2) {
        if (strcmp(tokens[operand_index+1], ",")) {
            *error = ERROR_MISSING_COMMA;
            return false;
        }
    }
    operand_index = FIRST_ARGUMENT;

    /* Check each operand for correctness */
    for (; operand_index < num_tokens; operand_index += 2)
    {
        if (!isLabel(tokens[operand_index], false))
        {
            *error = ERROR_INCORRECT_OPERAND_TYPE;
            return false;
        }

        if (entryLine && getLabelType(tokens[operand_index], *labels, error) == LABEL_TYPE_NOT_FOUND)
        {
             *error = ERROR_UNRECOGNIZED_LABEL;
             return false;
        }


    }
    operand_index = FIRST_ARGUMENT;

    /* Check for duplicated labels */
    return !(isDuplicatedLabel(labels, tokens[operand_index], label_type, error, is_first_itteration));
}

/**
 * @brief Determines if the label already exists in labels
 * 
 * @param labels Pointer to the head of the LabelNode linked list.
 * @param label_name array that stores name of the label
 * @param label_type stores type of the label
 * @param error Pointer to an Error variable for error handling.
 * @param is_first_itteration flag if the function is called from the first iteration
 * @return Returns true if the label already exists
 */
bool isDuplicatedLabel(LabelNode** labels, char* label_name, LabelType label_type, Error* error, bool is_first_itteration){
    LabelType argument_label_type = LABEL_TYPE_NOT_FOUND; /* the label type to be compared to ( checks if the label already exists in the linked list ) */
    argument_label_type =  getLabelType(label_name, *labels, error); /* gets the label type from the labels linked list */

    /*checks if entry label exists in other label types*/
    if ((label_type == LABEL_TYPE_ENTRY) && (argument_label_type == LABEL_TYPE_EXTERNAL) &&  argument_label_type != LABEL_TYPE_NOT_FOUND )
    {
        *error = ERROR_DUPLICATE_LABEL;
        return true;
    }

    /*checks if external label exists in other label types*/
    if ((label_type == LABEL_TYPE_EXTERNAL) && (argument_label_type != LABEL_TYPE_EXTERNAL) &&  argument_label_type != LABEL_TYPE_NOT_FOUND && is_first_itteration)
    {
        *error = ERROR_DUPLICATE_LABEL;
        return true;
    }
    /*checks if there are duplicate of labels in label types of CODE and DATA among themselfs*/
    else {
        switch (argument_label_type)
        {
        case LABEL_TYPE_CODE:
        case LABEL_TYPE_DATA:
            if (label_type == LABEL_TYPE_CODE || label_type == LABEL_TYPE_DATA) {
                *error = ERROR_DUPLICATE_LABEL;
                return true;
            }        
        default:
            return false;
            break;
        }
    }
}