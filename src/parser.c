#include <stdio.h>

#include "parser.h"
#include "scanner.h"

Token lookahead;
int has_lookahead = 0;

static const char *token_name(Token token);
//
static Token next_token(void);
static void match(Token expected);
static void syntax_error(Token expected, Token actual);

// Declarations for parsing functions
static void program(void);
static void statement_list(void);
static void statement(void);
static void expression(void);
static void primary(void);
static void add_op(void);
static void id_list(void);
static void expr_list(void);

// Returns the next token without consuming it.
Token next_token(void)
{
    if (!has_lookahead) {
        current_token = scanner();
        has_lookahead = 1;
    }

    return current_token;
}

// Checks and consumes the expected token.
static void match(Token expected)
{
    Token actual = next_token();

    if (actual == expected) {
        has_lookahead = 0;
    } else {
        syntax_error(expected, actual);
    }
}

// Reports an unexpected token as a syntax error.
static void syntax_error(Token expected, Token actual)
{
    fprintf(stderr,
            "Syntax error: expected token %s, but found %s\n",
            token_name(expected),
            token_name(actual));
}

// Returns the readable name of a token for when a syntax error occurs..
static const char *token_name(Token token)
{
    switch (token) {
        case BEGIN:      return "BEGIN";
        case END:        return "END";
        case READ:       return "READ";
        case WRITE:      return "WRITE";
        case ID:         return "ID";
        case INTLITERAL: return "INTLITERAL";
        case LPAREN:     return "LPAREN";
        case RPAREN:     return "RPAREN";
        case SEMICOLON:  return "SEMICOLON";
        case COMMA:      return "COMMA";
        case ASSIGNOP:   return "ASSIGNOP";
        case PLUSOP:     return "PLUSOP";
        case MINUSOP:    return "MINUSOP";
        case SCANEOF:    return "SCANEOF";
        default:         return "UNKNOWN";
    }
}

// Parses the complete input and verifies the end of file.
void system_goal(void)
{
    program();
    match(SCANEOF);
}

// Parses a Micro program enclosed by begin and end.
static void program(void)
{
    match(BEGIN);
    statement_list();
    match(END);
}

// Parses one or more Micro statements.
static void statement_list(void)
{
    statement();

    while (1) {
        switch (next_token()) {
            case ID:
            case READ:
            case WRITE:
                statement();
                break;

            default:
                return;
        }
    }
}

// Parses a Micro statement based on its first token.
static void statement(void)
{
    Token tok = next_token();

    switch (tok) {
        case ID:
            match(ID);
            match(ASSIGNOP);
            expression();
            match(SEMICOLON);
            break;

        case READ:
            match(READ);
            match(LPAREN);
            id_list();
            match(RPAREN);
            match(SEMICOLON);
            break;

        case WRITE:
            match(WRITE);
            match(LPAREN);
            expr_list();
            match(RPAREN);
            match(SEMICOLON);
            break;

        default:
            fprintf(stderr,
                    "Syntax error: expected ID, READ or WRITE, found %s\n",
                    token_name(tok));
            break;
    }
}

// Parses a comma separated list of identifiers.
static void id_list(void)
{
    match(ID);

    while (next_token() == COMMA) {
        match(COMMA);
        match(ID);
    }
}

// Parses an expression containing primary values and addition or subtraction operators.
static void expression(void)
{
    Token tok;

    primary();  

    while ((tok = next_token()) == PLUSOP || tok == MINUSOP) {
        add_op();
        primary();
    }
}

// Parses an addition or subtraction operator.
static void add_op(void)
{
    Token tok = next_token();

    if (tok == PLUSOP) {
        match(PLUSOP);
    } else if (tok == MINUSOP) {
        match(MINUSOP);
    } else {
        fprintf(stderr,
                "Syntax error: expected PLUSOP or MINUSOP, found %s\n",
                token_name(tok));
    }
}

// Parses an identifier, integer literal, or parenthesized expression.
static void primary(void)
{
    Token tok = next_token();

    switch (tok) {
        case ID:
            match(ID);
            break;

        case INTLITERAL:
            match(INTLITERAL);
            break;

        case LPAREN:
            match(LPAREN);
            expression();
            match(RPAREN);
            break;

        default:
            fprintf(stderr,
                    "Syntax error: expected ID, INTLITERAL or LPAREN, found %s\n",
                    token_name(tok));
            break;
    }
}

// Parses a comma separated list of expressions.
static void expr_list(void)
{
    expression();

    while (next_token() == COMMA) {
        match(COMMA);
        expression();
    }
}