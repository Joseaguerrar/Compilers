#include <stdio.h>
#include <ctype.h>

#include "scanner.h"

Token scanner(void)
{
    int in_char;

    // Read input while characters are available
    while ((in_char = getchar()) != EOF) {
        // Skip whitespace characters
        if (isspace(in_char))
        {
            continue;
        }
       
    }
    
    return SCANEOF;
}