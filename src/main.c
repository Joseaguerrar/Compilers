#include <stdio.h>

#include "scanner.h"

int main(void)
{
    Token token = scanner();

    if (token == SCANEOF) {
        printf("SCANEOF\n");
    }

    return 0;
}