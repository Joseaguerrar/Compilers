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
    TOK_INLINE, TOK_RESTRICT, TOK_ALIGNAS, TOK_ALIGNOF, TOK_ATOMIC, TOK_BOOL, TOK_COMPLEX, TOK_GENERIC,
    TOK_IMAGINARY, TOK_NORETURN, TOK_STATIC_ASSERT, TOK_THREAD_LOCAL,

    // Identifiers, Constants
    TOK_IDENTIFIER,
    TOK_INT_CONST,
    TOK_FLOAT_CONST,
    TOK_CHAR_CONST,
    TOK_STRING_LITERAL,

    // Operators
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PERCENT,
    TOK_ASSIGN, TOK_EQ, TOK_NEQ, TOK_LT, TOK_GT, TOK_LE, TOK_GE,
    TOK_AND, TOK_OR, TOK_NOT, TOK_BIT_AND, TOK_BIT_OR, TOK_BIT_XOR,
    TOK_BIT_NOT, TOK_LSHIFT, TOK_RSHIFT, TOK_INCREMENT, TOK_DECREMENT, TOK_ARROW,

    TOK_PLUS_ASSIGN, TOK_MINUS_ASSIGN, TOK_STAR_ASSIGN, TOK_SLASH_ASSIGN, TOK_PERCENT_ASSIGN,
    TOK_BIT_AND_ASSIGN, TOK_BIT_OR_ASSIGN, TOK_BIT_XOR_ASSIGN, TOK_LSHIFT_ASSIGN, TOK_RSHIFT_ASSIGN,

    // Punctuators
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_LBRACKET, TOK_RBRACKET, TOK_SEMICOLON, TOK_COMMA, TOK_DOT,
    TOK_COLON, TOK_QUESTION, TOK_ELLIPSIS, TOK_HASH, TOK_DOUBLE_HASH, 

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
int init_scanner(const char* filename);

// Close scanner input
void close_scanner(void);

#endif