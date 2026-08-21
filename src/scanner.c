#include <stdio.h>
#include <ctype.h>

#include "scanner.h"

Token scanner(void)
{
    //TODO: manage lexical errors
    int in_char;
    int c;

    // Read input while characters are available
    while ((in_char = getchar()) != EOF) {

        // Skip whitespace characters
        if (isspace(in_char))
        {
            continue;
        }

        // Check if it is digit
        if (isdigit(in_char)) {

            while (isdigit(c = getchar())) {
                // Continue reading digits
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
        }
    }
    
    return SCANEOF;
}