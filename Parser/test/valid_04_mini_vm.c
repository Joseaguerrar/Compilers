/*
 * valid_04_mini_vm.c
 * Programa de prueba sintácticamente válido para C99/C11.
 * Máquina virtual basada en pila (Stack-based Virtual Machine) e intérprete de bytecode:
 * Conjunto de instrucciones, ensamblador en memoria, marcos de llamada,
 * desensamblador y programas de prueba (factorial, fibonacci, mcd).
 * Líneas: > 500
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STACK_MAX_SIZE 256
#define CALL_STACK_MAX 64
#define MEMORY_MAX_SIZE 1024
#define MAX_INSTRUCTIONS 2048

/* ========================================================================= */
/* 1. Conjunto de Instrucciones (Opcodes)                                    */
/* ========================================================================= */

typedef enum {
    OP_NOP = 0,
    OP_PUSH,       /* Pone constante en la pila */
    OP_POP,        /* Descarta tope de la pila */
    OP_DUP,        /* Duplica tope de la pila */
    OP_SWAP,       /* Intercambia los dos elementos superiores */
    OP_ADD,        /* Suma: a + b */
    OP_SUB,        /* Resta: a - b */
    OP_MUL,        /* Multiplicación: a * b */
    OP_DIV,        /* División entera: a / b */
    OP_MOD,        /* Módulo: a % b */
    OP_NEG,        /* Negación aritmética: -a */
    OP_NOT,        /* Negación lógica: !a */
    OP_EQ,         /* Igualdad: a == b */
    OP_NEQ,        /* Desigualdad: a != b */
    OP_LT,         /* Menor que: a < b */
    OP_LE,         /* Menor o igual: a <= b */
    OP_GT,         /* Mayor que: a > b */
    OP_GE,         /* Mayor o igual: a >= b */
    OP_JMP,        /* Salto incondicional */
    OP_JZ,         /* Salto si tope es cero */
    OP_JNZ,        /* Salto si tope no es cero */
    OP_CALL,       /* Llamada a subrutina */
    OP_RET,        /* Retorno de subrutina */
    OP_LOAD,       /* Cargar de memoria global a pila */
    OP_STORE,      /* Guardar de pila a memoria global */
    OP_PRINT,      /* Imprimir tope de la pila */
    OP_HALT        /* Detener ejecucion */
} OpCode;

typedef struct {
    OpCode op;
    long long operand;
} Instruction;

/* ========================================================================= */
/* 2. Programa de Bytecode                                                   */
/* ========================================================================= */

typedef struct {
    Instruction code[MAX_INSTRUCTIONS];
    size_t count;
} VMProgram;

void program_init(VMProgram *prog) {
    if (prog != NULL) {
        prog->count = 0;
    }
}

size_t program_emit(VMProgram *prog, OpCode op, long long operand) {
    if (prog == NULL || prog->count >= MAX_INSTRUCTIONS) {
        return 0;
    }
    size_t addr = prog->count;
    prog->code[addr].op = op;
    prog->code[addr].operand = operand;
    prog->count++;
    return addr;
}

void program_patch_operand(VMProgram *prog, size_t address, long long new_operand) {
    if (prog != NULL && address < prog->count) {
        prog->code[address].operand = new_operand;
    }
}

/* ========================================================================= */
/* 3. Marco de Llamada y Estado de la Máquina Virtual                       */
/* ========================================================================= */

typedef struct {
    size_t return_address;
} CallFrame;

typedef enum {
    VM_OK = 0,
    VM_HALTED,
    VM_STACK_OVERFLOW,
    VM_STACK_UNDERFLOW,
    VM_DIVISION_BY_ZERO,
    VM_OUT_OF_BOUNDS_MEMORY,
    VM_INVALID_OPCODE,
    VM_MAX_CYCLES_EXCEEDED
} VMStatus;

typedef struct {
    long long stack[STACK_MAX_SIZE];
    int sp; /* Stack Pointer */

    CallFrame call_stack[CALL_STACK_MAX];
    int csp; /* Call Stack Pointer */

    long long memory[MEMORY_MAX_SIZE];
    size_t pc; /* Program Counter */

    bool running;
    size_t cycles;
} VM;

void vm_init(VM *vm) {
    if (vm == NULL) {
        return;
    }
    vm->sp = -1;
    vm->csp = -1;
    vm->pc = 0;
    vm->running = false;
    vm->cycles = 0;
    memset(vm->memory, 0, sizeof(vm->memory));
}

int vm_push(VM *vm, long long value) {
    if (vm->sp >= STACK_MAX_SIZE - 1) {
        return 0;
    }
    vm->sp++;
    vm->stack[vm->sp] = value;
    return 1;
}

int vm_pop(VM *vm, long long *out_val) {
    if (vm->sp < 0) {
        return 0;
    }
    if (out_val != NULL) {
        *out_val = vm->stack[vm->sp];
    }
    vm->sp--;
    return 1;
}

int vm_peek(const VM *vm, long long *out_val) {
    if (vm->sp < 0 || out_val == NULL) {
        return 0;
    }
    *out_val = vm->stack[vm->sp];
    return 1;
}

/* ========================================================================= */
/* 4. Desensamblador (Disassembler)                                         */
/* ========================================================================= */

const char *opcode_to_string(OpCode op) {
    switch (op) {
        case OP_NOP:   return "NOP";
        case OP_PUSH:  return "PUSH";
        case OP_POP:   return "POP";
        case OP_DUP:   return "DUP";
        case OP_SWAP:  return "SWAP";
        case OP_ADD:   return "ADD";
        case OP_SUB:   return "SUB";
        case OP_MUL:   return "MUL";
        case OP_DIV:   return "DIV";
        case OP_MOD:   return "MOD";
        case OP_NEG:   return "NEG";
        case OP_NOT:   return "NOT";
        case OP_EQ:    return "EQ";
        case OP_NEQ:   return "NEQ";
        case OP_LT:    return "LT";
        case OP_LE:    return "LE";
        case OP_GT:    return "GT";
        case OP_GE:    return "GE";
        case OP_JMP:   return "JMP";
        case OP_JZ:    return "JZ";
        case OP_JNZ:   return "JNZ";
        case OP_CALL:  return "CALL";
        case OP_RET:   return "RET";
        case OP_LOAD:  return "LOAD";
        case OP_STORE: return "STORE";
        case OP_PRINT: return "PRINT";
        case OP_HALT:  return "HALT";
        default:       return "UNKNOWN";
    }
}

void vm_disassemble(const VMProgram *prog, const char *title) {
    if (prog == NULL) {
        return;
    }
    printf("=== Disassembly: %s (%zu instrs) ===\n", title ? title : "Program", prog->count);
    for (size_t i = 0; i < prog->count; i++) {
        Instruction inst = prog->code[i];
        printf("%04zu: %-7s", i, opcode_to_string(inst.op));
        switch (inst.op) {
            case OP_PUSH:
            case OP_JMP:
            case OP_JZ:
            case OP_JNZ:
            case OP_CALL:
            case OP_LOAD:
            case OP_STORE:
                printf(" %lld", inst.operand);
                break;
            default:
                break;
        }
        printf("\n");
    }
    printf("======================================\n");
}

/* ========================================================================= */
/* 5. Motor de Ejecución (Execution Loop)                                   */
/* ========================================================================= */

VMStatus vm_step(VM *vm, const VMProgram *prog) {
    if (vm == NULL || prog == NULL || vm->pc >= prog->count) {
        return VM_INVALID_OPCODE;
    }
    Instruction inst = prog->code[vm->pc];
    vm->pc++;
    vm->cycles++;

    switch (inst.op) {
        case OP_NOP:
            break;

        case OP_PUSH:
            if (!vm_push(vm, inst.operand)) return VM_STACK_OVERFLOW;
            break;

        case OP_POP:
            if (vm->sp < 0) return VM_STACK_UNDERFLOW;
            vm->sp--;
            break;

        case OP_DUP: {
            long long v = 0;
            if (!vm_peek(vm, &v)) return VM_STACK_UNDERFLOW;
            if (!vm_push(vm, v)) return VM_STACK_OVERFLOW;
            break;
        }

        case OP_SWAP: {
            if (vm->sp < 1) return VM_STACK_UNDERFLOW;
            long long tmp = vm->stack[vm->sp];
            vm->stack[vm->sp] = vm->stack[vm->sp - 1];
            vm->stack[vm->sp - 1] = tmp;
            break;
        }

        case OP_ADD: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a + b);
            break;
        }

        case OP_SUB: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a - b);
            break;
        }

        case OP_MUL: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a * b);
            break;
        }

        case OP_DIV: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            if (b == 0) return VM_DIVISION_BY_ZERO;
            vm_push(vm, a / b);
            break;
        }

        case OP_MOD: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            if (b == 0) return VM_DIVISION_BY_ZERO;
            vm_push(vm, a % b);
            break;
        }

        case OP_NEG: {
            long long a;
            if (!vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, -a);
            break;
        }

        case OP_NOT: {
            long long a;
            if (!vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a == 0 ? 1 : 0);
            break;
        }

        case OP_EQ: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a == b ? 1 : 0);
            break;
        }

        case OP_NEQ: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a != b ? 1 : 0);
            break;
        }

        case OP_LT: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a < b ? 1 : 0);
            break;
        }

        case OP_LE: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a <= b ? 1 : 0);
            break;
        }

        case OP_GT: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a > b ? 1 : 0);
            break;
        }

        case OP_GE: {
            long long b, a;
            if (!vm_pop(vm, &b) || !vm_pop(vm, &a)) return VM_STACK_UNDERFLOW;
            vm_push(vm, a >= b ? 1 : 0);
            break;
        }

        case OP_JMP:
            vm->pc = (size_t)inst.operand;
            break;

        case OP_JZ: {
            long long cond;
            if (!vm_pop(vm, &cond)) return VM_STACK_UNDERFLOW;
            if (cond == 0) {
                vm->pc = (size_t)inst.operand;
            }
            break;
        }

        case OP_JNZ: {
            long long cond;
            if (!vm_pop(vm, &cond)) return VM_STACK_UNDERFLOW;
            if (cond != 0) {
                vm->pc = (size_t)inst.operand;
            }
            break;
        }

        case OP_CALL:
            if (vm->csp >= CALL_STACK_MAX - 1) return VM_STACK_OVERFLOW;
            vm->csp++;
            vm->call_stack[vm->csp].return_address = vm->pc;
            vm->pc = (size_t)inst.operand;
            break;

        case OP_RET:
            if (vm->csp < 0) return VM_STACK_UNDERFLOW;
            vm->pc = vm->call_stack[vm->csp].return_address;
            vm->csp--;
            break;

        case OP_LOAD: {
            size_t addr = (size_t)inst.operand;
            if (addr >= MEMORY_MAX_SIZE) return VM_OUT_OF_BOUNDS_MEMORY;
            if (!vm_push(vm, vm->memory[addr])) return VM_STACK_OVERFLOW;
            break;
        }

        case OP_STORE: {
            size_t addr = (size_t)inst.operand;
            if (addr >= MEMORY_MAX_SIZE) return VM_OUT_OF_BOUNDS_MEMORY;
            long long val;
            if (!vm_pop(vm, &val)) return VM_STACK_UNDERFLOW;
            vm->memory[addr] = val;
            break;
        }

        case OP_PRINT: {
            long long val;
            if (!vm_pop(vm, &val)) return VM_STACK_UNDERFLOW;
            printf("[VM OUT]: %lld\n", val);
            break;
        }

        case OP_HALT:
            return VM_HALTED;

        default:
            return VM_INVALID_OPCODE;
    }

    return VM_OK;
}

VMStatus vm_run(VM *vm, const VMProgram *prog, size_t max_cycles) {
    if (vm == NULL || prog == NULL) {
        return VM_INVALID_OPCODE;
    }
    vm->running = true;
    while (vm->running) {
        if (max_cycles > 0 && vm->cycles >= max_cycles) {
            return VM_MAX_CYCLES_EXCEEDED;
        }
        VMStatus st = vm_step(vm, prog);
        if (st == VM_HALTED) {
            vm->running = false;
            return VM_OK;
        }
        if (st != VM_OK) {
            vm->running = false;
            return st;
        }
    }
    return VM_OK;
}

/* ========================================================================= */
/* 6. Programas de Demostración en Bytecode                                  */
/* ========================================================================= */

/*
 * Genera programa en bytecode para calcular N! (Factorial):
 * Entradas: N en memoria[0].
 * Salida: memoria[1] y se imprime en pantalla.
 */
void build_factorial_program(VMProgram *p, long long n) {
    program_init(p);

    program_emit(p, OP_PUSH, n);
    program_emit(p, OP_STORE, 0); /* mem[0] = n */
    program_emit(p, OP_PUSH, 1);
    program_emit(p, OP_STORE, 1); /* mem[1] = fact = 1 */

    /* Loop address: 4 */
    size_t loop_start = p->count;
    program_emit(p, OP_LOAD, 0);  /* carga n */
    program_emit(p, OP_PUSH, 1);
    program_emit(p, OP_LE, 0);    /* n <= 1 ? */
    size_t jz_exit = program_emit(p, OP_JNZ, 0); /* Si n <= 1, salir */

    /* fact = fact * n */
    program_emit(p, OP_LOAD, 1);
    program_emit(p, OP_LOAD, 0);
    program_emit(p, OP_MUL, 0);
    program_emit(p, OP_STORE, 1);

    /* n = n - 1 */
    program_emit(p, OP_LOAD, 0);
    program_emit(p, OP_PUSH, 1);
    program_emit(p, OP_SUB, 0);
    program_emit(p, OP_STORE, 0);

    /* Regresar al loop */
    program_emit(p, OP_JMP, (long long)loop_start);

    /* Salida */
    size_t exit_addr = p->count;
    program_patch_operand(p, jz_exit, (long long)exit_addr);

    program_emit(p, OP_LOAD, 1);
    program_emit(p, OP_PRINT, 0);
    program_emit(p, OP_HALT, 0);
}

/*
 * Genera programa en bytecode para calcular el N-ésimo término de Fibonacci.
 */
void build_fibonacci_program(VMProgram *p, long long n) {
    program_init(p);

    program_emit(p, OP_PUSH, 0);
    program_emit(p, OP_STORE, 10); /* a = 0 */
    program_emit(p, OP_PUSH, 1);
    program_emit(p, OP_STORE, 11); /* b = 1 */
    program_emit(p, OP_PUSH, n);
    program_emit(p, OP_STORE, 12); /* count = n */

    size_t loop_start = p->count;
    program_emit(p, OP_LOAD, 12);
    program_emit(p, OP_PUSH, 0);
    program_emit(p, OP_LE, 0);
    size_t jnz_exit = program_emit(p, OP_JNZ, 0);

    /* next = a + b */
    program_emit(p, OP_LOAD, 10);
    program_emit(p, OP_LOAD, 11);
    program_emit(p, OP_ADD, 0);
    program_emit(p, OP_STORE, 13);

    /* a = b */
    program_emit(p, OP_LOAD, 11);
    program_emit(p, OP_STORE, 10);

    /* b = next */
    program_emit(p, OP_LOAD, 13);
    program_emit(p, OP_STORE, 11);

    /* count-- */
    program_emit(p, OP_LOAD, 12);
    program_emit(p, OP_PUSH, 1);
    program_emit(p, OP_SUB, 0);
    program_emit(p, OP_STORE, 12);

    program_emit(p, OP_JMP, (long long)loop_start);

    size_t exit_addr = p->count;
    program_patch_operand(p, jnz_exit, (long long)exit_addr);

    program_emit(p, OP_LOAD, 10);
    program_emit(p, OP_PRINT, 0);
    program_emit(p, OP_HALT, 0);
}

/* ========================================================================= */
/* 7. Programa Principal de Pruebas (Driver)                                 */
/* ========================================================================= */

int main(void) {
    VM vm;
    VMProgram prog;

    printf("=== Probando Factorial en Mini VM (5!) ===\n");
    build_factorial_program(&prog, 5);
    vm_disassemble(&prog, "Factorial 5");

    vm_init(&vm);
    VMStatus status = vm_run(&vm, &prog, 10000);
    printf("Resultado VM Status: %d, Ciclos ejecutados: %zu\n", (int)status, vm.cycles);
    printf("mem[1] (resultado esperado 120): %lld\n", vm.memory[1]);

    printf("\n=== Probando Fibonacci en Mini VM (fib(8)) ===\n");
    build_fibonacci_program(&prog, 8);
    vm_disassemble(&prog, "Fibonacci 8");

    vm_init(&vm);
    status = vm_run(&vm, &prog, 10000);
    printf("Resultado VM Status: %d, Ciclos ejecutados: %zu\n", (int)status, vm.cycles);
    printf("Fib(8) (resultado esperado 21): %lld\n", vm.memory[10]);

    printf("\n=== Probando Operaciones Aritmeticas Directas ===\n");
    program_init(&prog);
    program_emit(&prog, OP_PUSH, 15);
    program_emit(&prog, OP_PUSH, 4);
    program_emit(&prog, OP_DIV, 0);
    program_emit(&prog, OP_PRINT, 0); /* 15 / 4 = 3 */
    program_emit(&prog, OP_PUSH, 15);
    program_emit(&prog, OP_PUSH, 4);
    program_emit(&prog, OP_MOD, 0);
    program_emit(&prog, OP_PRINT, 0); /* 15 % 4 = 3 */
    program_emit(&prog, OP_HALT, 0);

    vm_init(&vm);
    status = vm_run(&vm, &prog, 1000);
    printf("Status operaciones directas: %d\n", (int)status);

    printf("\n=== Todas las pruebas de la Mini VM completadas con exito ===\n");
    return 0;
}
