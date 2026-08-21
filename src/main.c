#include <stdio.h>

#include "scanner.h"

int main(void)
{
    Token token;

    do {
        token = scanner();

        switch (token) {

            case INTLITERAL:
                printf("INTLITERAL\n");
                break;

            case LPAREN:
                printf("LPAREN\n");
                break;

            case RPAREN:
                printf("RPAREN\n");
                break;

            case SEMICOLON:
                printf("SEMICOLON\n");
                break;

            case COMMA:
                printf("COMMA\n");
                break;

            case PLUSOP:
                printf("PLUSOP\n");
                break;

            case SCANEOF:
                printf("SCANEOF\n");
                break;

            default:
                printf("UNKNOWN TOKEN\n");
                break;
        }

    } while (token != SCANEOF);

    return 0;
}