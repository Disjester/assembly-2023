#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_LINE_LENGTH 80
#define MAX_TOKENS 10
#define NUMBER_OF_COMMANDS 16
#define NUM_OF_REGISTERS 8

#define ADDRESING_NUMBER 0
#define ADDRESING_LABEL 1 
#define ADDRESING_REGISTER 2

enum dot_type{
    DOT_OTHER,
    DOT_DATA,
    DOT_STRING, 
    DOT_ENTRY, 
    DOT_EXTERN
};      

#define DEFAULT_EXTERN_MEMORY -1

#define MAX_MEMORY_SIZE 1024
/**
 * @brief a boolean type , that can hold either true or false;
 * 
 */
typedef enum bool{
    false = 0,
    true = 1
}bool;

typedef enum LabelType {
    LABEL_TYPE_CODE,
    LABEL_TYPE_DATA,
    LABEL_TYPE_EXTERNAL,
    LABEL_TYPE_ENTRY
} LabelType;

typedef enum OperandType{
    OPERAND_TYPE_OTHER,
    OPERAND_TYPE_LABEL,
    OPERAND_TYPE_REGISTER,
    OPERAND_TYPE_NUMBER
} OperandType;

#endif