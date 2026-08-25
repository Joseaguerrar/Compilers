#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

extern char token_buffer[];

// Scans the input and returns token type.
Token scanner(void);

#endif