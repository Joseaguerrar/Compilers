/**
 * @file scanner.c
 * @brief Implementation of the Micro lexical analyzer.
 *
 * The scanner reads characters from standard input and groups them into
 * tokens.
 */

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "scanner.h"

char token_buffer[BUFFER_SIZE];

/** @brief Position at which the next character will be appended to the token buffer. */
static int buffer_index = 0;

/**
 * @brief Classifies an accumulated lexeme as a reserved word or an identifier.
 *
 * Reserved words are spelled like identifiers, so the buffer is compared
 * against the reserved words of the language once the lexeme is complete.
 *
 * @return The token class of the reserved word, or ::ID when the lexeme is not
 *         reserved.
 */
static Token check_reserved(void);

/**
 * @brief Reports a character that cannot begin any token.
 *
 * The message is written to standard error because standard output carries the
 * generated assembly code.
 *
 * @param c The incorrect character.
 */
static void lexical_error(int c);

static Token check_reserved(void)
{
    if (strcmp(token_buffer, "begin") == 0) {
        return BEGIN;
    } else if (strcmp(token_buffer, "end") == 0) {
        return END;
    } else if (strcmp(token_buffer, "read") == 0) {
        return READ;
    } else if (strcmp(token_buffer, "write") == 0) {
        return WRITE;
    }
    
    return ID;
}

static void lexical_error(int c)
{
    fprintf(stderr, "Lexical error: invalid character '%c'\n", c);
}

void clear_buffer(void)
{
    buffer_index = 0;
    token_buffer[0] = '\0';
}

void buffer_char(int c)
{
    if (buffer_index < BUFFER_SIZE - 1) {
        token_buffer[buffer_index] = (char)c;
        buffer_index++;
        token_buffer[buffer_index] = '\0';
    }
}

Token scanner(void)
{
    int in_char;
    int c;

    clear_buffer();

    while ((in_char = getchar()) != EOF) {

        if (isspace(in_char))
        {
            continue;
        }

        if (isdigit(in_char)) {

            buffer_char(in_char);

            while (isdigit(c = getchar())) {
                buffer_char(c);
            }

            if (c != EOF) {
                ungetc(c, stdin); 
            }

            return INTLITERAL;
        }

        if (in_char == '(') {
            return LPAREN;
        }

        if (in_char == ')') {
            return RPAREN;
        }

        if (in_char == ';') {
            return SEMICOLON;
        }

        if (in_char == ',') {
            return COMMA;
        }

        if (in_char == '+') {
            return PLUSOP;
        }

        if (in_char == '|') {
            return CONDITIONALOP;
        }

        if (in_char == '-') {
            c = getchar();

            if (c == '-') {

                while ((c = getchar()) != '\n' && c != EOF) {
                }

                continue;
            }

            if (c != EOF) {
                ungetc(c, stdin);
            }

            return MINUSOP;
        }

        if (in_char == ':') {
            c = getchar();

            if (c == '=') {
                return ASSIGNOP;
            }

            if (c != EOF) {
                ungetc(c, stdin);
            }

            lexical_error(in_char);
            continue;
        }

        if (isalpha(in_char)) {

            buffer_char(in_char);

            while (isalnum(c = getchar()) || c == '_') {
                buffer_char(c);
            }

            if (c != EOF) {
                ungetc(c, stdin);
            }

            return check_reserved();
        }

        else {
            lexical_error(in_char);
        }
    }
    
    return SCANEOF;
}
