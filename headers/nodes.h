typedef struct MacroNode {
    char* macro_name;
    struct CodeNode* code_node;
    struct MacroNode* next;
} MacroNode;

typedef struct CodeNode {
    char* code_row;
    struct CodeNode* next;
} CodeNode;