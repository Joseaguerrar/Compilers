#include <stdio.h>

#include "codegen.h"

// Generates a formatted target instruction.
void generate(const char *op_code,
              const char *operand1,
              const char *operand2,
              const char *result)
{
    printf("%s", op_code);

    if (operand1[0] != '\0') {
        printf(" %s", operand1);
    }

    if (operand2[0] != '\0') {
        printf(",%s", operand2);
    }

    if (result[0] != '\0') {
        printf(",%s", result);
    }

    printf("\n");
}