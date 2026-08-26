#include <stdio.h>

#include "parser.h"
#include "scanner.h"
#include "semantic.h"

Token current_token;
static int has_lookahead = 0;

static const char *token_name(Token token);
//
static Token next_token(void);
static void match(Token expected);
static void syntax_error(Token expected, Token actual);

// Declarations for parsing functions
static void program(void);
static void statement_list(void);
static void statement(void);
static expr_rec expression(void);
static expr_rec primary(void);
static op_rec add_op(void);
static void id_list(void);
static void expr_list(void);

static expr_rec identifier(void);

// Returns the next token without consuming it.
static Token next_token(void)
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
        case ID: {
            expr_rec target;
            expr_rec source;

            target = identifier();
            match(ASSIGNOP);
            source = expression();
            assign(target, source);
            match(SEMICOLON);
            break;
        }
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

// Parses a comma separated list of identifiers and generates read operations.
static void id_list(void)
{
    expr_rec id;

    id = identifier();
    read_id(id);

    while (next_token() == COMMA) {
        match(COMMA);
        id = identifier();
        read_id(id);
    }
}

// Parses an expression and returns its semantic record.
static expr_rec expression(void)
{
    expr_rec left;
    expr_rec right;
    op_rec op;
    Token tok;

    left = primary();

    while ((tok = next_token()) == PLUSOP || tok == MINUSOP) {
        op = add_op();
        right = primary();
        left = gen_infix(left, op, right);
    }

    return left;
}

// Parses an addition or subtraction operator and returns its semantic record.
static op_rec add_op(void)
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

    return process_op();
}

// Parses an identifier, integer literal, or parenthesized expression.
static expr_rec primary(void)
{
    Token tok = next_token();
    expr_rec result;
    switch (tok) {
        case ID:
            return identifier();
            break;

        case INTLITERAL:
            match(INTLITERAL);
            result = process_literal();
            break;

        case LPAREN:
            match(LPAREN);
            result = expression();
            match(RPAREN);
            break;

        default:
            fprintf(stderr,
                    "Syntax error: expected ID, INTLITERAL or LPAREN, found %s\n",
                    token_name(tok));
            break;
    }

    return result;
}

// Parses a comma separated list of expressions and generates write operations.
static void expr_list(void)
{
    expr_rec expr;

    expr = expression();
    write_expr(expr);

    while (next_token() == COMMA) {
        match(COMMA);
        expr = expression();
        write_expr(expr);
    }
}

// Parses an identifier and returns its semantic record.
static expr_rec identifier(void)
{
    match(ID);
    return process_id();
}