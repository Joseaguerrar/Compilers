#include <string.h>

#include "symbol_table.h"

#define MAX_SYMBOLS 100

static string symbol_table[MAX_SYMBOLS];
static int symbol_count = 0;

// Checks whether a name is already in the symbol table.
int lookup(string s)
{
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i], s) == 0) {
            return 1;
        }
    }

    return 0;
}

// Adds a name to the symbol table.
void enter(string s)
{
    if (symbol_count < MAX_SYMBOLS) {
        strcpy(symbol_table[symbol_count], s);
        symbol_count++;
    }
}

// Return the count of the symbol table
int get_symbol_count(void) {
    return symbol_count;
}

// Uses the symbol count to iterate on the symbol table and then return the symbol based on the index given
const char *get_symbol(int index) {
    if (index >= 0 && index < symbol_count) {
        return symbol_table[index];
    }
    return NULL;
}