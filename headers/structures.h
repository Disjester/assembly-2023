#ifndef STRUCTURES_H
#define STRUCTURES_H
 
#define MAX_COMMAND_LENGTH 20
#define NUMBER_OF_ADRESSING_METHODS 3 
#define NUM_OF_COMMANDS 16


typedef struct Command{
    char command[MAX_COMMAND_LENGTH]; /*command name*/
    short opcode;
    int number_of_operands;
    int sourceAddresingMethod [NUMBER_OF_ADRESSING_METHODS]; /*number, label, register*/
    int destinationAddresingMethod [NUMBER_OF_ADRESSING_METHODS]; /*number, label, register*/
} Command;

typedef struct MacroNode{
    char* macro_name;
    struct CodeNode* code_node;
    struct MacroNode* next;
} MacroNode;

typedef struct CodeNode{
    char* code_row;
    struct CodeNode* next;
} CodeNode;

typedef struct LabelNode{
    short memory_adress;
    char* label_name;
    LabelType label_type;
    struct LabelNode* next;
} LabelNode;

#endif
