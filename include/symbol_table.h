#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "semantic.h"

// Checks whether a name is already in the symbol table.
extern int lookup(string s);

// Adds a name to the symbol table.
extern void enter(string s);

#endif