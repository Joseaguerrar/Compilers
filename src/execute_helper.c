#include <stdio.h>
#include <stdlib.h>
#include "execute_helper.h"

// Execute the executable file given on params
int execute_program(const char *binary_filename)
{
    // Reserve the space on memory for a command of 512 bytes (512 chars), basically a comand that, at max will have 511 chars + \0 at end
    char command[512];

    // used snprintf instead of sprintf (it's safer) to build a solid command to pass to the system()
    snprintf(command, sizeof(command), "/.%s", binary_filename);

    // execute the command defined above, on a very safe way :D 
    int result = system(command);
    if (result != 0) {
        fprintf(stderr, "Error to execute %s\n", binary_filename);
        return 1; // Unix standart 0: (EXIT_SUCCESS) 1: (EXIT_FAILURE)
    }

    return 0;
}