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

    system_goal();

    return 0;
}