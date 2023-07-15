#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_COMMAND_LENGTH 20
#define NUMBER_OF_ADRESSING_METHODS 3 
#define NUM_OF_COMMANDS 16
typedef struct Command{
    char command[MAX_COMMAND_LENGTH]; 
    short opcode;
    int operand_number;
    int sourceAddresingMethod [NUMBER_OF_ADRESSING_METHODS];
    int destinationAddresingMethod [NUMBER_OF_ADRESSING_METHODS]; 
} Command;


#endif