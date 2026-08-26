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


// Start function x86-64
void gen_start(FILE *out) {
    fprintf(out, ".intel_syntax noprefix\n");
    fprintf(out, ".global main\n");
    fprintf(out, ".extern printf, scanf\n\n");
    
    /* Strings format I/O */
    fprintf(out, ".section .rodata\n");
    fprintf(out, "fmt_read:  .string \"%%d\"\n");
    fprintf(out, "fmt_write: .string \"%%d\\n\"\n\n");

    /* Start (main) */
    fprintf(out, ".section .text\n");
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
}

// Finish function x86-64
// void gen_finish(FILE *out) {
 
//     fprintf(out, "    mov eax, 0\n");
//     fprintf(out, "    mov rsp, rbp\n");
//     fprintf(out, "    pop rbp\n");
//     fprintf(out, "    ret\n\n");

//     /* VARIABLES AND TEMP ON .bss */
//     fprintf(out, ".section .bss\n");
//     // Walk by the symbol table and print each ID and temp:
//     // fprintf(out, "    %s: .zero 4\n", symbol_name);
// }