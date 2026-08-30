/**
 * @file codegen.c
 * @brief Implementation of the x86-64 code translator of the Micro compiler.
 *
 * The generated x86-64 program is a single @c main function that calls @c scanf and
 * @c printf for input and output. All variables and temporaries are global
 * objects reserved in the @c .bss section.
 */

#include <stdio.h>

#include "codegen.h"
#include "symbol_table.h"

void gen_start(FILE *out) 
{
    fprintf(out, ".intel_syntax noprefix\n");
    fprintf(out, ".global main\n");
    fprintf(out, ".extern printf, scanf\n\n");

    fprintf(out, ".section .rodata\n");
    fprintf(out, "fmt_read:  .string \"%%d\"\n");
    fprintf(out, "fmt_write: .string \"%%d\\n\"\n\n");

    fprintf(out, ".section .text\n");
    fprintf(out, "main:\n");
    fprintf(out, "    push rbp\n");
    fprintf(out, "    mov rbp, rsp\n");
}

void gen_finish(FILE *out) 
{
    fprintf(out, "    mov eax, 0\n");
    fprintf(out, "    mov rsp, rbp\n");
    fprintf(out, "    pop rbp\n");
    fprintf(out, "    ret\n\n");
}

void gen_symbol_table(FILE *out)
{
    fprintf(out, ".section .bss\n");
    for (int i = 0 ; i < get_symbol_count() ; i++) {
        fprintf(out, "    %s: .zero 4\n", get_symbol(i));
    }
}

void gen_read(FILE *out, const char *name)
{
    fprintf(out, "    lea rsi, [%s]\n", name);
    fprintf(out, "    lea rdi, [fmt_read]\n");
    fprintf(out, "    mov eax, 0\n");
    fprintf(out, "    call scanf\n\n");
}

void gen_write(FILE *out, expr_rec out_expr)
{
    if (out_expr.kind == LITERALEXPR) {
        fprintf(out, "    mov esi, %d\n", out_expr.val);
    } else {
        fprintf(out, "    mov esi, [%s]\n", out_expr.name);
    }
    fprintf(out, "    lea rdi, [fmt_write]\n");
    fprintf(out, "    mov eax, 0\n");
    fprintf(out, "    call printf\n\n");
}

void gen_assign(FILE *out, const char *target, expr_rec source) 
{
    if (source.kind == LITERALEXPR) {
        fprintf(out, "    mov dword ptr [%s], %d\n\n", target, source.val);
    } else {
        fprintf(out, "    mov eax, dword ptr [%s]\n", source.name);
        fprintf(out, "    mov dword ptr [%s], eax\n\n", target);
    }
}

void gen_infix_op(FILE *out, expr_rec e1, op_rec op, expr_rec e2, const char *target)
{
    if (e1.kind == LITERALEXPR) {
        fprintf(out, "    mov eax, %d\n", e1.val);
    } else {
        fprintf(out, "    mov eax, dword ptr [%s]\n", e1.name);
    }

    const char *instruction = (op.operator == PLUS) ? "add" : "sub";
    if (e2.kind == LITERALEXPR) {
        fprintf(out, "    %s eax, %d\n", instruction, e2.val);
    } else {
        fprintf(out, "    %s eax, dword ptr [%s]\n", instruction, e2.name);
    }

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
