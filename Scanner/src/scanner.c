#include "token.h"
#include <stdio.h>
#include <stdlib.h>

extern FILE* yyin;
extern int yylex(void);
extern Token current_token;

void init_scanner(const char* filename) {
    FILE* fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error al abrir el archivo para escaneo: %s\n", filename);
        exit(1);
    }
    yyin = fp;
}

Token Get_Token(void) {
    yylex();
    return current_token;
}
