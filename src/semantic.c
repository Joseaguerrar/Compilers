// FROM BOOK pages 43, 45

#include <stdio.h>
#include <string.h>

#include "semantic.h"
#include "symbol_table.h"
#include "codegen.h"
#include "parser.h"

static const char *extract_op(op_rec op);

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

// Extracts the target opcode from an operator record.
static const char *extract_op(op_rec op)
{
    if (op.operator == PLUS) {
        return "Add";
    }

    return "Sub";
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

// Produces an operator semantic record.
op_rec process_op(void)
{
    op_rec o;

    if (current_token == PLUSOP) {
        o.operator = PLUS;
    } else {
        o.operator = MINUS;
    }

    return o;
}

// Generates code for an infix expression and returns its temporary result.
expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2)
{
    expr_rec e_rec;
    string left;
    string right;

    e_rec.kind = TEMPEXPR;
    strcpy(e_rec.name, get_temp());

    strcpy(left, extract(e1));
    strcpy(right, extract(e2));

    generate(extract_op(op), left, right, e_rec.name);

    return e_rec;
}