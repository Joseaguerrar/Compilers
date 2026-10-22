#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>

#include "preprocessor.h"

int parse_file(const char *input_filename, int *syntax_errors);

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo_fuente>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char temp_filename[PATH_MAX];
    int lexical_errors = 0;
    int preprocessor_errors = 0;

    if (!preprocess_file(
            argv[1],
            temp_filename,
            sizeof(temp_filename),
            &lexical_errors,
            &preprocessor_errors)) {
        fprintf(stderr, "Error fatal durante el preproceso\n");
        return EXIT_FAILURE;
    }

    int syntax_errors = 0;
    int ok = parse_file(temp_filename, &syntax_errors);

    /* Delete the temp file to free memory */
    unlink(temp_filename);

    printf("Errores léxicos de preproceso: %d\n", lexical_errors);
    printf("Errores de preprocesador: %d\n", preprocessor_errors);
    printf("Errores de sintaxis: %d\n", syntax_errors);

    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}