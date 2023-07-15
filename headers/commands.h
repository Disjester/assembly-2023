#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_COMMAND_LENGTH 20
#define NUMBER_OF_ADRESSING_METHODS 3 
#define NUM_OF_COMMANDS 16
typedef struct Command{
    char command[MAX_COMMAND_LENGTH]; /*command name*/
    short opcode;
    int operand_number;
    int sourceAddresingMethod [NUMBER_OF_ADRESSING_METHODS]; /*number, label, register*/
    int destinationAddresingMethod [NUMBER_OF_ADRESSING_METHODS]; /*number, label, register*/
} Command;


#endif