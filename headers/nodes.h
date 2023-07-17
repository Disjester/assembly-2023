#ifndef NODES_H
#define NODES_H

typedef struct {
    char* macro_name;
    struct CodeNode* code_node;
    struct MacroNode* next;
} MacroNode;

typedef struct {
    char* code_row;
    struct CodeNode* next;
} CodeNode;

typedef struct {
    short memory_adress;
    char* label_name;
    LabelType label_type;
    struct LabelNode* next;
} LabelNode;

#endif
