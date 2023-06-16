/**
 * @file nodes.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2023-06-16
 * 
 * @copyright Copyright (c) 2023
 * 
 */

typedef struct MacroNode {
    char* macro_name;
    struct CodeNode* code_node;
    struct MacroNode* macro_node;
} MacroNode;

typedef struct CodeNode {
    char* code_row;
    struct CodeNode* next;
} CodeNode;