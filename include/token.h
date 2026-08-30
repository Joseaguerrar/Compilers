/**
 * @file token.h
 * @brief Token classes recognized by the Micro scanner.
 */

#ifndef TOKEN_H
#define TOKEN_H

/**
 * @brief Enumerates every token class of the Micro language.
 *
 * The scanner returns one of these values for each lexeme it recognizes, and
 * the parser selects its productions according to the value returned.
 */
typedef enum token_types{
    BEGIN, /**< Reserved word @c begin, which opens the program body. */
    END, /**< Reserved word @c end, which closes the program body. */
    READ, /**< Reserved word @c read, which introduces an input statement. */
    WRITE, /**< Reserved word @c write, which introduces an output statement. */
    ID, /**< Identifier: a letter followed by letters, digits, or underscores. */
    INTLITERAL, /**< Integer literal: a non-empty sequence of digits. */
    LPAREN, /**< Left parenthesis @c ( . */
    RPAREN, /**< Right parenthesis @c ) . */
    SEMICOLON, /**< Statement terminator @c ; . */
    COMMA, /**< List separator @c , . */
    ASSIGNOP, /**< Assignment operator @c := . */
    PLUSOP, /**< Addition operator @c + . */
    MINUSOP, /**< Subtraction operator @c - . */
    CONDITIONALOP, /**< Separator @c | of the conditional expression @c (E1|E2|E3). */
    SCANEOF /**< End of the input stream. */
} Token;

#endif
