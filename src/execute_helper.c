/**
 * @file execute_helper.c
 * @brief Execution of the program produced by the compiler.
 */

#include <stdio.h>
#include <stdlib.h>
#include "execute_helper.h"

int execute_program(const char *binary_filename)
{
    char command[512];

    snprintf(command, sizeof(command), "./%s", binary_filename);

    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error to execute %s\n", binary_filename);
        return 1;
    }

    return 0;
}
