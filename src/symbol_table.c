/**
 * @file symbol_table.c
 * @brief Implementation of the Micro symbol table.
 *
 * The table is a fixed-size array of names searched linearly. This is adequate
 * for Micro, whose programs are small and whose identifiers carry no attribute
 * other than their spelling.
 */

#include <string.h>

#include "symbol_table.h"

/** @brief Maximum number of names the symbol table can hold. */
#define MAX_SYMBOLS 1000

/** @brief Storage for the names entered into the table. */
static string symbol_table[MAX_SYMBOLS];

/** @brief Number of names currently held in the table. */
static int symbol_count = 0;

int lookup(string s)
{
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i], s) == 0) {
            return 1;
        }
    }

    return 0;
}

void enter(string s)
{
    if (symbol_count < MAX_SYMBOLS) {
        strcpy(symbol_table[symbol_count], s);
        symbol_count++;
    }
}

int get_symbol_count(void) {
    return symbol_count;
}

const char *get_symbol(int index) {
    if (index >= 0 && index < symbol_count) {
        return symbol_table[index];
    }
    return NULL;
}
