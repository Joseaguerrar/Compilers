#ifndef CODEGEN_H
#define CODEGEN_H

// Generates a formatted target instruction.
void generate(const char *op_code,
              const char *operand1,
              const char *operand2,
              const char *result);

// Generates the standart start of a assembly program
void gen_start(FILE *out);

// Generates the standart finish of a assembly program
void gen_finish(FILE *out);

// Generates the symbol tables at the finish of a assembly program
void gen_symbol_table(FILE *out);

// Generates the stantard code sequence on assembly to write
void gen_read(FILE *out, const char *name);

// Generates the stantard code sequence on assembly to assing the source to the target
void gen_write(FILE *out, expr_rec out_expr);

// Generates the stantard code sequence on assembly to handle the aritmetics operations
void gen_infix_op(FILE *out, expr_rec e1, op_rec op, expr_rec e2, const char *target);

#endif