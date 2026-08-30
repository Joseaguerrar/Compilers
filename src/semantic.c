/**
 * @file semantic.c
 * @brief Implementation of the semantic action routines of the Micro compiler.
 *
 * These routines are called directly by the parser. They maintain the symbol
 * table, allocate the temporaries required to hold intermediate results, and
 * decide whether an operation can be resolved at compile time or must be
 * translated into instructions by the code generator.
 */

#include <stdio.h>
#include <string.h>

#include "semantic.h"
#include "symbol_table.h"
#include "codegen.h"
#include "parser.h"
#include "scanner.h"

/**
 * @brief Enters a name if it is not yet in the the symbol table.
 *
 * Micro has no explicit declarations, so every identifier and every generated
 * temporary is declared implicitly the first time it is used.
 *
 * @param s The name to be declared.
 */
static void check_id(string s);

/**
 * @brief Allocates a new temporary and declares it in the symbol table.
 *
 * Temporaries are named @c Temp_1, @c Temp_2, and so on, in the order in which
 * they are requested.
 *
 * @return The name of the new temporary.
 */
static char *get_temp(void);

static void check_id(string s)
{
    if (!lookup(s)) {
        enter(s);
    }
}

static char *get_temp(void)
{
    static int max_temp = 0;
    static char tempname[MAXIDLEN];

    max_temp++;
    sprintf(tempname, "Temp_%d", max_temp);
    check_id(tempname);

    return tempname;
}

void start(void)
{
    gen_start(stdout);
}

void finish(void)
{
    gen_finish(stdout);
    gen_symbol_table(stdout);
}

void assign(expr_rec target, expr_rec source)
{
    gen_assign(stdout, target.name, source);
}

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

expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2)
{
    expr_rec e_rec;

    if (e1.kind == LITERALEXPR && e2.kind == LITERALEXPR) {
        e_rec.kind = LITERALEXPR;
        e_rec.val = (op.operator == PLUS) ? (e1.val + e2.val) : (e1.val - e2.val);
        return e_rec;
    }

    e_rec.kind = TEMPEXPR;
    strcpy(e_rec.name, get_temp());
    gen_infix_op(stdout, e1, op, e2, e_rec.name);

    return e_rec;
}

expr_rec gen_cond(expr_rec e1, expr_rec e2, expr_rec e3) {

    expr_rec e_rec;

    if (e1.kind == LITERALEXPR) {
        return (e1.val != 0) ? e2 : e3;
    }

    e_rec.kind = TEMPEXPR;
    strcpy(e_rec.name, get_temp());
    gen_cond_op(stdout, e1, e2, e3, e_rec.name);

    return e_rec;
}

void read_id(expr_rec in_var)
{
    gen_read(stdout, in_var.name);
}

expr_rec process_id(void)
{
    expr_rec t;

    check_id(token_buffer);

    t.kind = IDEXPR;
    strcpy(t.name, token_buffer);

    return t;
}

expr_rec process_literal(void)
{
    expr_rec t;

    t.kind = LITERALEXPR;
    (void) sscanf(token_buffer, "%d", &t.val);

    return t;
}

void write_expr(expr_rec out_expr)
{
    gen_write(stdout, out_expr);
}
