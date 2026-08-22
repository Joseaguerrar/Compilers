#ifndef CODEGEN_H
#define CODEGEN_H

// Generates a formatted target instruction.
void generate(const char *op_code,
              const char *operand1,
              const char *operand2,
              const char *result);

#endif