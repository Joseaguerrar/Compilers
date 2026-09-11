/**
 * @file compile_helper.c
 * @brief Assembly and linking of the program produced by the compiler.
 */

#include <stdio.h>
#include <stdlib.h>
#include "compile_helper.h"

int compile_program(const char *s_filename)
{
    char command[512];

    snprintf(command, sizeof(command), "gcc -no-pie %s -o program_mostro_binary", s_filename);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error to assembly and link %s with gcc\n", s_filename);
        return 1;
    }

    return 0;
}
