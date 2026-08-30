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
 * This function implements the goal symbol of the grammar: it parses the
 * program body, verifies that the input ends immediately afterwards, and
 * invokes the semantic routines that emit the start, the body, and the
 * end of the target program.
 */
void system_goal(void);

#endif
