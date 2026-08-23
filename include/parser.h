#ifndef PARSER_H
#define PARSER_H

#include "token.h"

extern Token current_token;

// Parses a complete Micro program
void system_goal(void);

#endif