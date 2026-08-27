#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "semantic.h"

// Checks whether a name is already in the symbol table.
extern int lookup(string s);

// Adds a name to the symbol table.
extern void enter(string s);

// Get the count of the symbol table, to iterate on it
int get_symbol_count(void);

// Get the whole symbol tables, on char *
const char *get_symbol(int index);

#endif