#ifndef SEMANTIC_H
#define SEMANTIC_H

#define MAXIDLEN 33
typedef char string[MAXIDLEN];

typedef struct operator { // For operators
    enum op { PLUS, MINUS } operator;
} op_rec;

// Expression types
enum expr { IDEXPR, LITERALEXPR, TEMPEXPR };

// For primary and expression
typedef struct expression {
    enum expr kind;
    union {
        string name; // For IDEXPR, TEMPEXPR
        int val; // For LITERALEXPR
    };
    
} expr_rec;

void start(void);
void finish(void);

#endif