#include <stdio.h>
#include "token.h"

extern void init_scanner(FILE *f);
extern Token Get_Token(void);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <archivo.c>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f) {
        perror("Error al abrir archivo");
        return 1;
    }

    init_scanner(f);

    Token tok;
    do {
        tok = Get_Token();
        printf("[Línea %d, Col %d] Token ID: %d | Cat: %d | Lexema: '%s'\n", 
               tok.line, tok.column, tok.type, tok.category, tok.lexeme);
    } while (tok.type != TOK_EOF);

    fclose(f);
    return 0;
}