#include <stdio.h>
#include "token.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo.c>\n", argv[0]);
        return 1;
    }

    init_scanner(argv[1]);

    Token tok;
    do {
        tok = Get_Token();
        printf("[Línea %2d, Col %2d] Token Code: %2d | Categoria: %d | Lexema: '%s'\n", 
               tok.line, tok.column, tok.code, tok.category, tok.lexeme);
    } while (tok.code != TOK_EOF);

    return 0;
}