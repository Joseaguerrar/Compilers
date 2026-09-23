#include <stdio.h>
#include <stdlib.h>

#include "token.h"

extern FILE *sc_in;
extern int sc_lex(void);

extern Token current_token;
extern int line_num;
extern int col_num;

static FILE *scanner_file = NULL;


int init_scanner(const char *filename) {

    if (filename == NULL) {
        return 0;
    }

    scanner_file = fopen(filename, "r");

    if (scanner_file == NULL) {

        fprintf(
            stderr,
            "Error: no se pudo abrir %s para scanning\n",
            filename
        );

        return 0;
    }

    line_num = 1;
    col_num = 1;

    sc_in = scanner_file;

    return 1;
}


Token Get_Token(void) {

    sc_lex();

    return current_token;
}


void close_scanner(void) {

    if (scanner_file != NULL) {

        fclose(scanner_file);

        scanner_file = NULL;
        sc_in = NULL;
    }
}