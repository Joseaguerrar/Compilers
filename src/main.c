/**
 * @file main.c
 * @brief Main of the Micro compiler.
 *
 * Redirects the standard streams so that the scanner reads the
 * source program file path from standard input and the code generator writes the target
 * program to standard output, then parses the program and hands the resulting
 * assembly file to the assembly compiler.
 */

#include <stdio.h>

#include "parser.h"
#include "compile_helper.h"
#include "execute_helper.h"

/**
 * @brief Compiles, assembles, and runs a Micro source program.
 *
 * @param argc Number of command-line arguments, which must be exactly two.
 * @param argv Argument vector, whose second element is the path of the Micro
 *             source file.
 * @return 0 on success, or 1 if the arguments are malformed or a file cannot
 *         be opened.
 */
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source.micro>\n", argv[0]);
        return 1;
    }

    if (freopen(argv[1], "r", stdin) == NULL) {
        perror("Error opening source file");
        return 1;
    }

    if (freopen("program_mostro.s", "w", stdout) == NULL) {
        perror("Error creating output '.s' file");
        return 1;
    }

    system_goal();

    fflush(stdout);

    compile_program("program_mostro.s");

    execute_program("program_mostro_binary");

    return 0;
}
