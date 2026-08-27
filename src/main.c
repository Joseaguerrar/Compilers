#include <stdio.h>

#include "parser.h"

// Opens the Micro source file and starts the compilation.
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

    // Gen the .s file 

    system_goal();

    // We must use gcc (system() on linux) to assembly and link the executable

    // Then we have to use system() to execute the x86 file
    return 0;
}