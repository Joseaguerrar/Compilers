#include <stdio.h>

#include "codegen.h"
#include "symbol_table.h"

// Generates a formatted target instruction. Deprecated?
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


// Generates the start function x86-64
void gen_start(FILE *out) 
{
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

// Generates the finish function x86-64
void gen_finish(FILE *out) 
{
    // STATIC, ALWAYS THE SAME, TO AVOID SEG FAULT
    fprintf(out, "    mov eax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n\n");
}

// Generates the symbol table
void gen_symbol_table(FILE *out)
{
    fprintf(out, ".section .bss\n");
    for (int i = 0 ; i < get_symbol_count() ; i++) { // symbol_count from symbol_table.c
        fprintf(out, "    %s: .zero 4\n", get_symbol(i));
    }
}

// Generates the stantard code sequence on assembly to read 
void gen_read(FILE *out, const char *name)
{
    fprintf(out, "    lea rsi, [%s]\n", name);
    fprintf(out, "    lea rdi, [fmt_read]\n");
    fprintf(out, "    mov eax, 0\n");
    fprintf(out, "    call scanf\n\n");
}

// Generates the stantard code sequence on assembly to write
void gen_write(FILE *out, expr_rec out_expr)
{
    if (out_expr.kind == LITERALEXPR) {
        fprintf(out, "    mov esi, %d\n", out_expr.val);
    } else {
        fprintf(out, "    mov esi, [%s]\n", out_expr.name);
    }
    fprintf(out, "    lea rdi, [fmt_write]\n");
    fprintf(out, "    mov eax, 0\n"); // Not using SSE / Floating
    fprintf(out, "    call printf\n\n");
}

// Generates the stantard code sequence on assembly to assing the source to the target
void gen_assign(FILE *out, const char *target, expr_rec source) 
{
    if (source.kind == LITERALEXPR) {
        fprintf(out, "    mov dword ptr [%s], %d\n\n", target, source.val);
    } else {
        fprintf(out, "    mov eax, dword ptr [%s]\n", source.name);
        fprintf(out, "    mov dword ptr [%s], eax\n\n", target);
    }
}

// Generates the stantard code sequence on assembly to handle the aritmetics operations
void gen_infix_op(FILE *out, expr_rec e1, op_rec op, expr_rec e2, const char *target)
{
    // The first op to eax
    if (e1.kind == LITERALEXPR) {
        fprintf(out, "    mov eax, %d\n", e1.val);
    } else {
        fprintf(out, "    mov eax, dword ptr [%s]\n", e1.name);
    }

    // Apply the sum or substraction
    // NOTE: REMEMBER, x86-64 2 directions
    // That means: add eax, ebx -> eax = eax + ebx
    const char *instruction = (op.operator == PLUS) ? "add" : "sub";
    if (e2.kind == LITERALEXPR) {
        fprintf(out, "    %s eax, %d\n", instruction, e2.val);
    } else {
        fprintf(out, "    %s eax, dword ptr [%s]\n", instruction, e2.name);
    }

    // Save the result on the temporal variable [target]
    fprintf(out, "    mov dword ptr [%s], eax\n\n", target);
}

void gen_cond_op(FILE *out, expr_rec e1, expr_rec e2, expr_rec e3, const char *target) {
    if (e1.kind == LITERALEXPR) {
        fprintf(out, "    mov ecx, %d\n", e1.val);
    } else {
        fprintf(out, "    mov ecx, dword ptr [%s]\n", e1.name);
    }

    if (e3.kind == LITERALEXPR) {
        fprintf(out, "    mov eax, %d\n", e3.val);
    } else {
        fprintf(out, "    mov eax, dword ptr [%s]\n", e3.name);
    }

    if (e2.kind == LITERALEXPR) {
        fprintf(out, "    mov edx, %d\n", e2.val);
    } else {
        fprintf(out, "    mov edx, dword ptr [%s]\n", e2.name);
    }

    fprintf(out, "    test ecx, ecx\n");
    fprintf(out, "    cmovne eax, edx\n");

    fprintf(out, "    mov dword ptr [%s], eax\n\n", target);
}
