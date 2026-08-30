/**
 * @file scanner.h
 * @brief Interface of the Micro lexical analyzer.
 */

#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

/** @brief Capacity of the token buffer, including the terminating null character. */
#define BUFFER_SIZE 33

/**
 * @brief Holds the lexeme of the token most recently returned by scanner().
 *
 * Only identifiers and integer literals populate this buffer. It is cleared at the
 * start of every call to scanner().
 */
extern char token_buffer[BUFFER_SIZE];

/**
 * @brief Reads the next token from standard input.
 *
 * White space is skipped and comments, which extend from @c -- to the end of
 * the line, are discarded. When a character cannot begin any token, the
 * scanner reports a lexical error and resumes scanning at the next character.
 *
 * @return The class of the token recognized, or SCANEOF at end of input.
 */
Token scanner(void);

/**
 * @brief Empties the token buffer.
 */
void clear_buffer(void);

/**
 * @brief Appends a character to the token buffer.
 *
 * Characters that would overflow the buffer are discarded, which truncates
 * lexemes longer than the maximum length of an identifier.
 *
 * @param c The character to be appended.
 */
void buffer_char(int c);
#endif
