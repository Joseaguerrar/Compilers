// FROM BOOK pages 43, 45

#include <stdio.h>
#include "semantic.h"
#include "symbol_table.h"
#include "codegen.h"

// Declares an identifier if it is not already in the symbol table.
static void check_id(string s)
{
    if (!lookup(s)) {
        enter(s);
        generate("Declare", s, "Integer", "");
    }
}

// Generates a new temporary identifier.
static char *get_temp(void)
{
    static int max_temp = 0;
    static char tempname[MAXIDLEN];

    max_temp++;
    sprintf(tempname, "Temp&%d", max_temp);
    check_id(tempname);

    return tempname;
}

// Extracts the string representation of an expression record.
static char *extract(expr_rec source)
{
    static char buffer[MAXIDLEN];

    if (source.kind == LITERALEXPR) {
        sprintf(buffer, "%d", source.val);
        return buffer;
    }

    return source.name;
}

// Initializes semantic processing.
void start(void)
{
}

// Generates code to finish the target program.
void finish(void)
{
    generate("Halt", "", "", "");
}

// Generates code for an assignment.
void assign(expr_rec target, expr_rec source)
{
    generate("Store", extract(source), target.name, "");
}