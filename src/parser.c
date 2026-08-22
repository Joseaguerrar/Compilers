#include <stdio.h>

#include "parser.h"
#include "scanner.h"

static Token lookahead;
static int has_lookahead = 0;

static Token next_token(void);
static void match(Token expected);
static void syntax_error(Token token);

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
static Token next_token(void)
{
    if (!has_lookahead) {
        lookahead = scanner();
        has_lookahead = 1;
    }

    return lookahead;
}

// Checks and consumes the expected token.
static void match(Token expected)
{
    Token actual = next_token();

    if (actual == expected) {
        has_lookahead = 0;
    } else {
        syntax_error(actual);
    }
}

// Reports an unexpected token as a syntax error.
static void syntax_error(Token token)
{
    fprintf(stderr, "Syntax error: unexpected token %d\n", token);
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
            syntax_error(tok);
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