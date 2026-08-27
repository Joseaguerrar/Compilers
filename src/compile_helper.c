#include <stdio.h>
#include <stdlib.h>
#include "compile_helper.h"

// Compile the ".s" to ".o" file and then the executable file
int compile_program(const char *s_filename)
{
    // Reserve the space on memory for a command of 512 bytes (512 chars), basically a comand that, at max will have 511 chars + \0 at end
    char command[512];

    // used snprintf instead of sprintf, it's safer
    snprintf(command, sizeof(command), "gcc -no-pie %s -o program", s_filename);

    // execute the command defined above, on a very safe way :D 
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error to assembly and link %s with gcc\n", s_filename);
        return 1; // Unix standart 0: (EXIT_SUCCESS) 1: (EXIT_FAILURE)
    }

    return 0;
}