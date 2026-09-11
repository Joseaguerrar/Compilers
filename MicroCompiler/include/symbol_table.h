/**
 * @file symbol_table.h
 * @brief Interface of the Micro symbol table.
 *
 * Micro declares every identifier implicitly and provides a single data type,
 * so the table records names alone. It stores both user identifiers and the
 * temporaries created by the semantic routines, and the code generator
 * uses it to reserve storage for each entry.
 */

#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "semantic.h"

/**
 * @brief Determines whether a name has already been entered into the table.
 *
 * @param s The name to search for.
 * @return 1 if the name is present, or 0 otherwise.
 */
extern int lookup(string s);

/**
 * @brief Adds a name to the symbol table.
 *
 * The request is ignored once the table is full.
 *
 * @param s The name to be added.
 */
extern void enter(string s);

/**
 * @brief Reports how many names the symbol table currently holds.
 *
 * @return The number of entries.
 */
int get_symbol_count(void);

/**
 * @brief Retrieves the name stored at a given position of the symbol table.
 *
 * @param index Position of the entry, based on zero.
 * @return The name stored at @p index, or @c NULL if the index is out of range.
 */
const char *get_symbol(int index);

#endif
