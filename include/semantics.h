// FROM BOOK Figure 2.8

#define MAXIDLEN 33
typedef char string[MAXIDLEN];

typedef struct operator {
    enum op { PLUS, MINUS } operator;
} op_rec;


enum expr { IDEXPR, LITERALEXPR, TEMPEXPR };


typedef struct expression {
    enum expr kind;
    union {
        string name; 
        int val; 
    };
} expr_rec;