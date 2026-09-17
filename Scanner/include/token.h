#ifndef TOKEN_H
#define TOKEN_H


// based on C documentation, lex category [Keywords, Identifiers, Constants, String, Operators & Punctuators, Errors]
typedef enum {
    CAT_KEYWORD,
    CAT_IDENTIFIER,
    CAT_CONSTANT,
    CAT_STRING_LITERAL,
    CAT_OPERATOR,
    CAT_PUNCTUATOR,
    CAT_ERROR,
    CAT_EOF
} TokenCategory;

typedef enum {
    TOK_EOF = 0,
    
    // Reservated
    TOK_AUTO, TOK_BREAK, TOK_CASE, TOK_CHAR, TOK_CONST, TOK_CONTINUE,
    TOK_DEFAULT, TOK_DO, TOK_DOUBLE, TOK_ELSE, TOK_ENUM, TOK_EXTERN,
    TOK_FLOAT, TOK_FOR, TOK_GOTO, TOK_IF, TOK_INT, TOK_LONG,
    TOK_REGISTER, TOK_RETURN, TOK_SHORT, TOK_SIGNED, TOK_SIZEOF,
    TOK_STATIC, TOK_STRUCT, TOK_SWITCH, TOK_TYPEDEF, TOK_UNION,
    TOK_UNSIGNED, TOK_VOID, TOK_VOLATILE, TOK_WHILE,

    // Identifiers, Constants
    TOK_IDENTIFIER,
    TOK_INT_CONST,
    TOK_FLOAT_CONST,
    TOK_CHAR_CONST,
    TOK_STRING_LITERAL,

    // Operators
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_ASSIGN, TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_AND, TOK_OR, TOK_NOT, TOK_ARROW, TOK_INCREMENT, TOK_DECREMENT,

    // Punctuators
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET, TOK_SEMICOLON, TOK_COMMA, TOK_DOT,

    // Error
    TOK_ERROR
} TokenType;

// Struct of the Token
typedef struct {
    TokenType code;
    TokenCategory category;
    char* lexeme;
    int line;
    int column;
} Token;

// Main token function
Token Get_Token(void);

// Initialize scanner based on a file
// TODO: Input filter on "filename" to avoid seg fault
void init_scanner(const char* filename);

#endif