// FROM BOOK pages 43, 45

#include <stdio.h>
#include <string.h>

#include "semantic.h"
#include "symbol_table.h"
#include "codegen.h"
#include "parser.h"
#include "scanner.h"

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
    sprintf(tempname, "Temp_%d", max_temp);
    check_id(tempname);

    return tempname;
}

// Extracts the string representation of an expression record.
static char *extract(expr_rec source)
{
    static char buffer[MAXIDLEN];

    if (source.kind == LITERALEXPR) {
        sprintf(buffer, "%d", source.val);
    } else {
        strcpy(buffer, source.name);
    }

    return buffer;
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
    //CALL THE START CODE GEN ON codegen.c
}

// Generates code to finish the target program.
void finish(void)
{
    //CALL THE FINISH CODE GEN ON codegen.c
    //CALL THE DUMP SYMBOL TABLE ON symbol_table.c
    generate("Halt", "", "", "");
}

// Generates code for an assignment.
void assign(expr_rec target, expr_rec source)
{
    //CALL THE ASSIGN CODE GEN ON codegen.c
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

// Generates code for reading an identifier.
void read_id(expr_rec in_var)
{
    //CALL THE READ CODE GEN ON codegen.c
    generate("Read", in_var.name, "Integer", "");
}

// Builds a semantic record for an identifier.
expr_rec process_id(void)
{
    expr_rec t;

    check_id(token_buffer);

    t.kind = IDEXPR;
    strcpy(t.name, token_buffer);

    return t;
}

// Builds a semantic record for an integer literal.
expr_rec process_literal(void)
{
    expr_rec t;

    t.kind = LITERALEXPR;
    (void) sscanf(token_buffer, "%d", &t.val);

    return t;
}

// Generates code for writing an expression.
void write_expr(expr_rec out_expr)
{
    //CALL THE WRITE CODE GEN ON codegen.c
    generate("Write", extract(out_expr), "Integer", "");
}