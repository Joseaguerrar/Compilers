#ifndef TOKEN_H
#define TOKEN_H

// Lex category, expand based on C documentation
typedef enum {
    TOK_INT, 
    TOK_WHILE, 
    TOK_IF, 
    TOK_IDENTIFIER, 
    TOK_NUMBER, 
    TOK_PLUS, 
    TOK_ASSIGN, 
    TOK_EOF, 
    TOK_ERROR
} TokenType;

// Struct of the Token
typedef struct {
    TokenType code; // Token
    char* lexeme;   // Lexeme
} Token;

// Main token function
Token Get_Token(void);

// Initialize scanner based on a file
// TODO: Input filter on "filename" to avoid seg fault
void init_scanner(const char* filename);

#endif