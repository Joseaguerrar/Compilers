/**
 * @file parser.c
 * @brief Implementation of the Micro recursive descent parser.
 *
 * Each non-terminal of the grammar is implemented by one function, and a
 * single token of lookahead is enough to select the production. The
 * parser calls the semantic routines directly as soon as it recognizes a
 * construct that requires translation.
 *
 * Syntax errors are reported but not repaired: the parser neither discards the
 * incorrect token nor abandons the translation; a malformed program still
 * produces an assembly file.
 */

#include <stdio.h>
#include <setjmp.h>

#include "parser.h"
#include "scanner.h"
#include "semantic.h"

Token current_token;

/** @brief Indicates whether current_token holds a token that has not been consumed. */
static int has_lookahead = 0;

/** @brief Buffer for the setjmp/longjmp mechanism. */
static jmp_buf parser_abort;

/**
 * @brief Converts a token class into a readable name for log messages.
 *
 * @param token The token class to be named.
 * @return A constant string naming the class, or @c "UNKNOWN" if the value
 * does not correspond to any token.
 */
static const char *token_name(Token token);

/**
 * @brief Inspects the next token without consuming it.
 *
 * The scanner is called only when no lookahead token is available, so repeated
 * calls return the same token.
 *
 * @return The class of the lookahead token.
 */
static Token next_token(void);

/**
 * @brief Consumes the lookahead token if it belongs to the expected class.
 *
 * When the classes differ a syntax error is reported and the token is left in
 * place.
 *
 * @param expected The token class required by the current production.
 */
static void match(Token expected);

/**
 * @brief Reports a token that does not match the current production.
 *
 * The message is written to standard error because standard output carries the
 * generated assembly code.
 *
 * @param expected The token class required by the production.
 * @param actual The token class actually found in the input.
 */
static void syntax_error(Token expected, Token actual);

/**
 * @brief Parses the body of a Micro program.
 *
 * The production is @c program @c -> @c begin @c statement_list @c end.
 */
static void program(void);

/**
 * @brief Parses a sequence of one or more statements.
 *
 * Statements are parsed while the lookahead token can begin one.
 */
static void statement_list(void);

/**
 * @brief Parses a single statement, selected by its first token.
 *
 * An identifier generates an assignment, and the reserved words @c read and
 * @c write introduce the input and output statements respectively. Any other
 * token is reported as a syntax error.
 */
static void statement(void);

/**
 * @brief Parses an expression and generates the code that evaluates it.
 *
 * The production is a sequence of primaries separated by add or minus operators.
 *
 * @return The semantic record describing the value of the expression.
 */
static expr_rec expression(void);

/**
 * @brief Parses a primary expression.
 *
 * A primary is an identifier, an integer literal, a parenthesized expression,
 * or the conditional expression @c (E1|E2|E3).
 *
 * @return The semantic record describing the value of the primary.
 */
static expr_rec primary(void);

/**
 * @brief Parses an add or minus operator.
 *
 * @return The semantic record describing the operator that was matched.
 */
static op_rec add_op(void);

/**
 * @brief Parses a comma separated list of identifiers.
 *
 * The code that reads a value into each identifier is generated as soon as the
 * identifier is recognized.
 */
static void id_list(void);

/**
 * @brief Parses a comma separated list of expressions.
 *
 * The code that writes each value is generated as soon as the corresponding
 * expression is recognized.
 */
static void expr_list(void);

/**
 * @brief Parses an identifier and builds its semantic record.
 *
 * The semantic routine is called immediately after the token is matched,
 * before the parser requests any further lookahead, because the name of
 * the identifier is held in a buffer that the scanner clears on every call.
 *
 * @return The semantic record describing the identifier.
 */
static expr_rec identifier(void);

static Token next_token(void)
{
    if (!has_lookahead) {
        current_token = scanner();
        has_lookahead = 1;

        if (scanner_had_error()) {
            longjmp(parser_abort, 1);
        }
    }

    return current_token;
}

static void match(Token expected)
{
    Token actual = next_token();

    if (actual == expected) {
        has_lookahead = 0;
    } else {
        syntax_error(expected, actual);
    }
}

static void syntax_error(Token expected, Token actual)
{
    fprintf(stderr,
            "Syntax error: expected token %s, but found %s\n",
            token_name(expected),
            token_name(actual));
    
    longjmp(parser_abort, 1);
}

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
        case CONDITIONALOP: return "CONDITIONALOP";
        case SCANEOF:    return "SCANEOF";
        default:         return "UNKNOWN";
    }
}

int system_goal(void)
{
    has_lookahead = 0;
    scanner_reset_error();

    if (setjmp(parser_abort) != 0) {
        return 1;
    }

    program();
    match(SCANEOF);
    finish();

    return 0;
}

static void program(void)
{
    start();
    match(BEGIN);
    statement_list();
    match(END);
}

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

            if (next_token() == CONDITIONALOP) {
                match(CONDITIONALOP);
                expr_rec result_two = expression();
                match(CONDITIONALOP);
                expr_rec result_three = expression();

                result = gen_cond(result, result_two, result_three);
            }
            
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

static expr_rec identifier(void)
{
    match(ID);
    return process_id();
}
