#ifndef COMMANDS_H
#define COMMANDS_H

#define MAX_COMMANDS 10
#define MAX_COMMAND_LENGTH 20
typedef struct Command{
    char command[MAX_COMMAND_LENGTH];
    short binaryValue[MAX_COMMAND_LENGTH];
} Command;

#endif