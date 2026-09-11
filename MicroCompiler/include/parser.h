/**
 * @file parser.h
 * @brief Interface of the Micro recursive descent parser.
 */

#ifndef PARSER_H
#define PARSER_H

#include "token.h"

/**
 * @brief The single token of lookahead currently held by the parser.
 *
 * The semantic routines inspect this variable to determine which operator the
 * parser has just matched.
 */
extern Token current_token;

/**
 * @brief Parses a complete Micro program and translates it to assembly code.
 *
 * @return 0 if the source program is valid, or 1 if a lexical or syntax
 *         error is detected.
 */
int system_goal(void);

#endif
