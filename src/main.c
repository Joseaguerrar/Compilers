/**
 * @file main.c
 * @brief Main of the Micro compiler.
 *
 * Redirects the standard streams so that the scanner reads the source
 * Micro program from standard input and the code generator writes the
 * generated x86-64 assembly to a file located in the same directory
 * as the original Micro source file.
 *
 * After translation, the generated assembly is assembled and linked
 * with GCC and the resulting executable is run only if compilation
 * succeeds.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "parser.h"
#include "compile_helper.h"
#include "execute_helper.h"

/**
 * @brief Builds the assembly filename associated with a Micro source file.
 *
 * The resulting file is placed in the same directory as the source file
 * and uses the same base name with the extension replaced by ".s".
 *
 * @param source_filename Path to the Micro source file.
 * @return Dynamically allocated path to the assembly file, or NULL if
 *         memory allocation fails.
 */
static char *build_assembly_filename(const char *source_filename)
{
    size_t source_length = strlen(source_filename);
    char *assembly_filename = malloc(source_length + 3);

    if (assembly_filename == NULL) {
        return NULL;
    }

    strcpy(assembly_filename, source_filename);

    char *last_slash = strrchr(assembly_filename, '/');
    char *last_dot = strrchr(assembly_filename, '.');

    if (last_dot != NULL &&
        (last_slash == NULL || last_dot > last_slash)) {
        strcpy(last_dot, ".s");
    } else {
        strcat(assembly_filename, ".s");
    }

    return assembly_filename;
}

/**
 * @brief Compiles, assembles, and runs a Micro source program.
 *
 * @param argc Number of command-line arguments, which must be exactly two.
 * @param argv Argument vector, whose second element is the path of the Micro
 *             source file.
 * @return 0 on success, or 1 if an error occurs.
 */
int main(int argc, char *argv[])
{

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source.micro>\n", argv[0]);
        return 1;
    }

    char *assembly_filename = build_assembly_filename(argv[1]);

    if (assembly_filename == NULL) {
        fprintf(stderr, "Error allocating assembly filename\n");
        return 1;
    }

    int saved_stdin = dup(STDIN_FILENO);

    if (saved_stdin == -1) {
        perror("Error saving STDIN");
        free(assembly_filename);
        return 1;
    }

    int saved_stdout = dup(STDOUT_FILENO);

    if (saved_stdout == -1) {
        perror("Error saving STDOUT");
        close(saved_stdin);
        free(assembly_filename);
        return 1;
    }

    if (freopen(argv[1], "r", stdin) == NULL) {
        perror("Error opening source file");
        close(saved_stdin);
        close(saved_stdout);
        free(assembly_filename);
        return 1;
    }

    if (freopen(assembly_filename, "w", stdout) == NULL) {
        perror("Error creating assembly file");
        close(saved_stdin);
        close(saved_stdout);
        free(assembly_filename);
        return 1;
    }

    system_goal();

    if (fflush(stdout) == EOF) {
        perror("Error flushing assembly output");
        close(saved_stdin);
        close(saved_stdout);
        free(assembly_filename);
        return 1;
    }

    if (dup2(saved_stdin, STDIN_FILENO) == -1) {
        perror("Error restoring STDIN");
        close(saved_stdin);
        close(saved_stdout);
        free(assembly_filename);
        return 1;
    }

    if (dup2(saved_stdout, STDOUT_FILENO) == -1) {
        perror("Error restoring STDOUT");
        close(saved_stdin);
        close(saved_stdout);
        free(assembly_filename);
        return 1;
    }

    close(saved_stdin);
    close(saved_stdout);

    if (compile_program(assembly_filename) != 0) {
        free(assembly_filename);
        return 1;
    }

    free(assembly_filename);

    if (execute_program("program_mostro_binary") != 0) {
        return 1;
    }

    return 0;
}
