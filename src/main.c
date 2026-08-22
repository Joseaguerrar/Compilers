#include <stdio.h>

#include "scanner.h"

extern char token_buffer[];

int main(void)
{
    Token token;

    do {
        token = scanner();

        switch (token) {

            case BEGIN:
                printf("BEGIN\n");
                break;

            case END:
                printf("END\n");
                break;

            case READ:
                printf("READ\n");
                break;

            case WRITE:
                printf("WRITE\n");
                break;

            case ID:
                printf("ID: %s\n", token_buffer);
                break;

            case INTLITERAL:
                printf("INTLITERAL: %s\n", token_buffer);
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

            case ASSIGNOP:
                printf("ASSIGNOP\n");
                break;

            case PLUSOP:
                printf("PLUSOP\n");
                break;

            case MINUSOP:
                printf("MINUSOP\n");
                break;

            case SCANEOF:
                printf("SCANEOF\n");
                break;
        }

    } while (token != SCANEOF);

    return 0;
}