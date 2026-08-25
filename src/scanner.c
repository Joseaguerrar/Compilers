#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "scanner.h"

#define BUFFER_SIZE 33

char token_buffer[BUFFER_SIZE];
static int buffer_index = 0;

// Clear the token_buffer and reset the buffer_index
void clear_buffer(void)
{
    buffer_index = 0;
    token_buffer[0] = '\0';
}

// Add a character to the token_buffer and update the buffer_index
void buffer_char(int c)
{
    if (buffer_index < BUFFER_SIZE - 1) {
        token_buffer[buffer_index] = (char)c;
        buffer_index++;
        token_buffer[buffer_index] = '\0';
    }
}

// Check if the token_buffer contains a reserved word and return the corresponding token type
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

    return ID; // Not a reserved word, treat as identifier
}

// Handle lexical errors by printing an error message to stderr
static void lexical_error(int c)
{
    fprintf(stderr, "Lexical error: invalid character '%c'\n", c);
}

Token scanner(void)
{
    int in_char;
    int c;

    // Clear the token buffer before reading a new token
    clear_buffer();

    // Read input while characters are available
    while ((in_char = getchar()) != EOF) {

        // Skip whitespace characters
        if (isspace(in_char))
        {
            continue;
        }

        // Check if it is digit
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

        // Check single character tokens
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

        // Check for minus operator or comment
        if (in_char == '-') {
            c = getchar();

            // If another '-' follows, it is a comment
            if (c == '-') {

                // Ignore everything until newline or EOF
                while ((c = getchar()) != '\n' && c != EOF) {
                    // Skip comment characters
                }

                continue;
            }

            // It was only a minus operator
            if (c != EOF) {
                ungetc(c, stdin);
            }

            return MINUSOP;
        }

        // Check for assignment operator
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

        // Check if it is an identifier with following rule: LETTER { LETTER | DIGIT | _ }
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