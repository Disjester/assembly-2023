#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_COMMAND_LENGTH 20
#define NUMBER_OF_ADRESSING_METHODS 3 
typedef struct Command{
    char* command[MAX_COMMAND_LENGTH];
    int operand_number; 
    short opCode;
    int sourceAddresingMethod [NUMBER_OF_ADRESSING_METHODS];
    int destinationAddresingMethod [NUMBER_OF_ADRESSING_METHODS]; 
} Command;


#endif