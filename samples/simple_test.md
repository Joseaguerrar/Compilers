## Input

```
begin
    read(a, b);
    c := a + b - 10;
    write(c);
end
```

## Output

```
.intel_syntax noprefix
.global main
.extern printf, scanf

.section .rodata
fmt_read:  .string "%d"
fmt_write: .string "%d\n"

.section .text
main:
    push rbp
    mov rbp, rsp

    ; read(a, b);
    lea rsi, [a]
    lea rdi, [fmt_read]
    mov eax, 0
    call scanf

    lea rsi, [b]
    lea rdi, [fmt_read]
    mov eax, 0
    call scanf

    ; c := a + b - 10;
    mov eax, [a]
    add eax, [b]
    mov [Temp_1], eax

    mov eax, [Temp_1]
    sub eax, 10
    mov [Temp_2], eax

    mov eax, [Temp_2]
    mov [c], eax

    ; write(c);
    mov esi, [c]
    lea rdi, [fmt_write]
    mov eax, 0
    call printf

    ; finish / ret
    mov eax, 0
    mov rsp, rbp
    pop rbp
    ret

.section .bss
    a: .zero 4
    b: .zero 4
    c: .zero 4
    Temp_1: .zero 4
    Temp_2: .zero 4
```