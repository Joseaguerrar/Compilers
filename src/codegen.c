#include <stdio.h>

#include "codegen.h"
#include "symbol_table.h"

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
void gen_finish(FILE *out) {
    // STATIC, ALWAYS THE SAME, TO AVOID SEG FAULT
    fprintf(out, "    mov eax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n\n");
}

void gen_symbol_table(FILE *out){
    fprintf(out, ".section .bss\n");
    for (int i = 0 ; i < get_symbol_count() ; i++) { // symbol_count from symbol_table.c
        fprintf(out, "    %s: .zero 4\n", get_symbol(i));
    }
}