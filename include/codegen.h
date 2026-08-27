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

#endif