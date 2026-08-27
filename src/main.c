#include <stdio.h>

#include "parser.h"
#include "compile_helper.h"
#include "execute_helper.h"

// Opens the Micro source file and starts the compilation.
int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <source.micro>\n", argv[0]);
        return 1;
    }

    // Handle error opening the file
    if (freopen(argv[1], "r", stdin) == NULL) {
        perror("Error opening source file");
        return 1;
    }

    // Handle error creating the ".s" file, all the stdout will go to the ".s" file
    if (freopen("program_mostro.s", "w", stdout) == NULL) {
        perror("Error creating output '.s' file");
        return 1;
    }

    // Execute our compiler
    system_goal();

    // Make sure all the buffer on c to be writed on the file
    fflush(stdout);

    // Use the compile helper to get the executable file
    compile_program("program_mostro.s");

    // Use the execute helper to run the program
    execute_program("program_mostro_binary");

    return 0;
}