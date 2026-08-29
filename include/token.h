#ifndef TOKEN_H
#define TOKEN_H

// Token types for the Micro language
typedef enum token_types{
    BEGIN,
    END,
    READ,
    WRITE,
    ID,
    INTLITERAL,
    LPAREN,
    RPAREN,
    SEMICOLON,
    COMMA,
    ASSIGNOP,
    PLUSOP,
    MINUSOP,
    CONDITIONALOP,
    SCANEOF
} Token;

#endif