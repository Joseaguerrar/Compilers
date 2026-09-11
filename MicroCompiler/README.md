# Micro x86-64 Compiler - Build & Usage Guide

This repository contains the source code for **MicroC**, a compiler for the
Micro language implemented in C for Linux.

The compiler translates a Micro source program into x86-64 assembly, invokes
GCC internally to assemble and link the generated code, and executes the
resulting program.

The build process of the compiler itself is managed using GNU `make`.

---

## Prerequisites

- **GCC** (GNU Compiler Collection) with C11 support
- **GNU Make**
- **Linux x86-64**

---

## Project Structure

```text
include/
    codegen.h
    compile_helper.h
    execute_helper.h
    parser.h
    scanner.h
    semantic.h
    symbol_table.h
    token.h

src/
    codegen.c
    compile_helper.c
    execute_helper.c
    main.c
    parser.c
    scanner.c
    semantic.c
    symbol_table.c

tests/
    *.micro

.gitignore
Makefile
README.md
```

---

## Compilation Flow

The project has two different compilation stages.

First, the C source files that implement the compiler are compiled into the
`microc` executable:

```text
src/*.c
   |
   | GCC
   v
 microc
```

Then, `microc` can compile a Micro source program:

```text
program.micro
      |
      | microc
      v
program.s
      |
      | gcc -no-pie
      v
program_mostro_binary
      |
      v
 execution
```

The generated `.s` file is stored in the **same directory as the original
Micro source file** and uses the same base name.

For example:

```text
tests/simple_test.micro
```

produces:

```text
tests/simple_test.s
```

The executable generated from the assembly is named:

```text
program_mostro_binary
```

This binary is reused and replaced whenever another Micro program is
successfully compiled.

---

## Usage with `make`

### 1. Compile the Compiler

Build the `microc` executable and the object files under `obj/`:

```bash
make
```

or explicitly:

```bash
make all
```

This produces:

```text
microc
obj/
```

---

### 2. Run the Compiler

Run `microc` with a Micro source file as its only argument:

```bash
./microc tests/simple_test.micro
```

The general syntax is:

```bash
./microc <source.micro>
```

For example:

```bash
./microc tests/folding_test.micro
```

generates:

```text
tests/folding_test.s
```

The generated assembly is then assembled and linked internally using a command
equivalent to:

```bash
gcc -no-pie tests/folding_test.s -o program_mostro_binary
```

If GCC succeeds, `microc` executes:

```bash
./program_mostro_binary
```

The user does not need to execute these GCC or binary commands manually.

---

## Input and Output

Micro programs may use the `read` and `write` statements.

Example:

```text
begin
    read(a, b);
    c := a + b - 10;
    write(c);
end
```

Run it with:

```bash
./microc tests/simple_test.micro
```

Then enter the requested integer values:

```text
13
23
```

The program prints:

```text
26
```

because:

```text
13 + 23 - 10 = 26
```

---

## Generated Assembly

Each successfully compiled Micro source keeps its generated assembly file in
the same directory as the source.

For example:

```text
tests/simple_test.micro
tests/simple_test.s

tests/folding_test.micro
tests/folding_test.s

tests/conditional_test.micro
tests/conditional_test.s
```

The assembly can be inspected directly with:

```bash
cat tests/simple_test.s
```

or with line numbers:

```bash
nl -ba tests/simple_test.s
```

---

## Compilation Errors

If the Micro source contains a lexical or syntax error, the compiler reports
the error and the compilation is aborted.

For example, an invalid source may produce:

```text
Syntax error: expected token BEGIN, but found ID
Compilation failed: invalid Micro source program.
```

In this case:

- GCC is not invoked.
- The generated program is not executed.
- A partially generated assembly file is not kept as a valid result.
- A previously generated `program_mostro_binary` is not executed.

---

## Clean Build Files

Remove the compiler executable, object files, and temporary generated binary:

```bash
make clean
```

Then rebuild from scratch with:

```bash
make
```

Generated `.s` files associated with individual Micro source files may be
removed manually when they are no longer needed.

For example:

```bash
rm tests/simple_test.s
```

---

## Manual Compilation

If `make` is not available, the compiler can be built directly with GCC:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude -g src/*.c -o microc
```

---

## Manual Step-by-Step Build

### 1. Create the object directory

```bash
mkdir -p obj
```

### 2. Compile each source file

```bash
gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/codegen.c -o obj/codegen.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/compile_helper.c -o obj/compile_helper.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/execute_helper.c -o obj/execute_helper.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/main.c -o obj/main.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/parser.c -o obj/parser.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/scanner.c -o obj/scanner.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/semantic.c -o obj/semantic.o

gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/symbol_table.c -o obj/symbol_table.o
```

### 3. Link the compiler

```bash
gcc obj/*.o -o microc
```

### 4. Run the compiler

```bash
./microc tests/simple_test.micro
```

### 5. Clean manually

```bash
rm -rf obj microc program_mostro_binary
```

---

## Example Tests

### Basic arithmetic and I/O

```text
begin
    read(a, b);
    c := a + b - 10;
    write(c);
end
```

### Constant folding

```text
begin
    x := 10 + 20 + 30 - 5;
    write(x);
end
```

Expected output:

```text
55
```

### Conditional expression

```text
begin
    read(x);
    y := ( x | 100 | 200 );
    write(y);
end
```

The conditional expression:

```text
(E1 | E2 | E3)
```

evaluates to `E2` when `E1 != 0`, and to `E3` when `E1 == 0`.

Therefore:

```text
x = 0   -> 200
x != 0  -> 100
```

---

## Rebuilding From Scratch

A complete clean build can be performed with:

```bash
make clean
make
```

Then run any Micro test with:

```bash
./microc tests/<test_name>.micro
```