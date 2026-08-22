#include <stdio.h>

#include "parser.h"
#include "scanner.h"

static Token lookahead;
static int has_lookahead = 0;

static Token next_token(void);
static void match(Token expected);
static void syntax_error(Token token);

// Forward declarations for parsing functions
static void program(void);
static void statement_list(void);

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