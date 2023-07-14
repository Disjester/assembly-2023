#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_LINE_LENGTH 80
#define MAX_TOKENS 10

#define DOT_DATA 1
#define DOT_STRING 2 
#define DOT_ENTRY 3
#define DOT_EXTERN 4
#define DOT_OTHER 0

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

typedef enum label_type {
    LABEL_TYPE_CODE,
    LABEL_TYPE_DATA,
    LABEL_TYPE_EXTERNAL,
    LABEL_TYPE_ENTRY
} LabelType;

#endif