/**
 * @file codegen.h
 * @brief Translates to Micro to x86-64 assembly code.
 *
 * Every routine writes in x86-64 syntax to the stream it receives. Stream will
 * be standard output
 * 
 * Each routine inspects the @c kind field of the expression in order to
 * translate correctly depending of if it's a literal or memory.
 */

#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "semantic.h"

/**
 * @brief Produces the needed initial section and imports of the target program.
 *
 * Declares @c main, imports @c printf and
 * @c scanf, defines the format strings used for input and output, and
 * establishes the stack frame.
 *
 * @param out Stream that receives the generated assembly code.
 */
void gen_start(FILE *out);

/**
 * @brief Produces the needed ending of the target program.
 *
 * The epilogue sets the exit status to zero, restores the stack frame, and
 * returns.
 *
 * @param out Stream that receives the generated assembly code.
 */
void gen_finish(FILE *out);

/**
 * @brief Produces a storage declaration for every entry of the symbol table.
 *
 * Each name becomes a four byte zero initialized object in the @c .bss
 * section.
 *
 * @param out Stream that receives the generated assembly code.
 */
void gen_symbol_table(FILE *out);

/**
 * @brief Produces the calling sequence that reads an integer into a variable.
 *
 * @param out Stream that receives the generated assembly code.
 * @param name Name of the variable that receives the input value.
 */
void gen_read(FILE *out, const char *name);

/**
 * @brief Produces the calling sequence that writes the value of an expression.
 *
 * @param out Stream that receives the generated assembly code.
 * @param out_expr Semantic record of the expression to be written.
 */
void gen_write(FILE *out, expr_rec out_expr);

/**
 * @brief Produces the code that stores an expression into a variable.
 *
 * @param out Stream that receives the generated assembly code.
 * @param target Name of the variable that receives the value.
 * @param source Semantic record of the expression being assigned.
 */
void gen_assign(FILE *out, const char *target, expr_rec source);

/**
 * @brief Produces the code that evaluates an infix addition or subtraction.
 *
 * The left operand is loaded into @c eax, the operator is applied to the right
 * operand, and the result is stored in @p target.
 *
 * @param out Stream that receives the generated assembly code.
 * @param e1 Semantic record of the left operand.
 * @param op Semantic record of the operator.
 * @param e2 Semantic record of the right operand.
 * @param target Name of the location that receives the result.
 */
void gen_infix_op(FILE *out, expr_rec e1, op_rec op, expr_rec e2, const char *target);

/**
 * @brief Produces the code that evaluates the conditional expression @c (E1|E2|E3).
 *
 * The three operands are loaded into registers, the condition is tested, and a
 * conditional move selects @p e2 when the condition is non-zero and @p e3
 * otherwise. The result is stored in @p target.
 *
 * @param out Stream that receives the generated assembly code.
 * @param e1 Semantic record of the condition.
 * @param e2 Semantic record selected when the condition is non-zero.
 * @param e3 Semantic record selected when the condition is zero.
 * @param target Name of the location that receives the result.
 */
void gen_cond_op(FILE *out, expr_rec e1, expr_rec e2, expr_rec e3, const char *target);

#endif
