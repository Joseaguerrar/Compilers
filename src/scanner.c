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

// Book code:
//
// #include <stdio.h>
// #include <ctype.h>

// int in_char, c;

// while ((in_char = getchar()) != EOF) {
//     if (isspace(in_char))
//         continue;       /* do nothing */
//     else if (isalpha(in_char)) {
//         /*
//          * ID ::= LETTER | ID LETTER
//          *               | ID DIGIT
//          *               | ID UNDERSCORE
//          */
//         for (c = getchar(); isalnum(c) || c == '_';
//                 c = getchar())
//             ;
//         ungetc(c, stdin);

//         return ID;
//     } else if (isdigit(in_char)) {
//         /*
//          * INTLITERAL ::= DIGIT |
//          *                INTLITERAL DIGIT
//          */
//         while (isdigit((c = getchar())))
//             ;
//         ungetc(c, stdin);

//         return INTLITERAL;
//     } else
//         lexical_error(in_char);
// }