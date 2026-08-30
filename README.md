# Micro-x86_64 Compiler - Build & Usage Guide

This repository contains the source code for the **MicroC** compiler. The build process is managed using GNU `make`.

---

## Prerequisites

* **GCC** (GNU Compiler Collection) supporting `C11`
* **GNU Make**

---

## Usage with `make`

### 1. Compile the Project
Build the executable (`microc`) and object files (`obj/*.o`):
```bash
make
```
*or explicitly:*
```bash
make all
```

### 2. Run the Compiler
Execute the compiler with a sample `.micro` source file:
```bash
./microc tests/prueba.micro
```
The second argument, in this case "tests/prueba.micro" can be replaced with any other micro file

### 3. Clean Build Files
Remove the `obj/` directory and the `microc` executable:
```bash
make clean
```

---

## Manual Compilation

If `make` is not available on your system, you can compile all source files directly using `gcc`:

```bash
gcc -Wall -Wextra -std=c11 -Iinclude -g src/*.c -o microc
```

### Manual Step-by-Step Build

1. **Create the object directory:**
   ```bash
   mkdir -p obj
   ```

2. **Compile source files into object files:**
   ```bash
   gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/codegen.c -o obj/codegen.o
   gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/main.c -o obj/main.o
   gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/parser.c -o obj/parser.o
   gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/scanner.c -o obj/scanner.o
   gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/semantic.c -o obj/semantic.o
   gcc -Wall -Wextra -std=c11 -Iinclude -g -c src/symbol_table.c -o obj/symbol_table.o
   ```

3. **Link object files into executable:**
   ```bash
   gcc obj/*.o -o microc
   ```

4. **Clean generated files manually:**
   ```bash
   rm -rf obj microc
   ```