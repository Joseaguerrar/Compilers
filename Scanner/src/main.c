#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>

#include "preprocessor.h"
#include "token.h"
#include "beamer_generator.h"


#define INITIAL_TOKEN_CAPACITY 128

#define TEX_FILENAME \
    "presentation/presentation.tex"

#define PDF_FILENAME \
    "presentation/presentation.pdf"

#define LATEX_LOG_FILENAME \
    "presentation/pdflatex.out"

static void free_tokens(
    Token *tokens,
    size_t token_count
) {
    if (tokens == NULL) {
        return;
    }

    for (size_t i = 0; i < token_count; i++) {
        free(tokens[i].lexeme);
    }

    free(tokens);
}

static int scan_file(
    const char *filename,
    Token **tokens,
    size_t *token_count
) {
    if (filename == NULL ||
        tokens == NULL ||
        token_count == NULL) {

        return 0;
    }

    if (!init_scanner(filename)) {
        return 0;
    }

    size_t capacity = INITIAL_TOKEN_CAPACITY;
    size_t count = 0;

    Token *array = malloc(
        capacity * sizeof(Token)
    );

    if (array == NULL) {

        fprintf(
            stderr,
            "Error: no se pudo reservar memoria para tokens\n"
        );

        close_scanner();

        return 0;
    }

    while (1) {

        Token token = Get_Token();

        if (token.code == TOK_EOF) {

            free(token.lexeme);

            break;
        }

        if (count >= capacity) {

            size_t new_capacity = capacity * 2;

            Token *new_array = realloc(
                array,
                new_capacity * sizeof(Token)
            );

            if (new_array == NULL) {

                fprintf(
                    stderr,
                    "Error: no se pudo ampliar "
                    "el arreglo de tokens\n"
                );

                free(token.lexeme);

                close_scanner();

                free_tokens(
                    array,
                    count
                );

                return 0;
            }

            array = new_array;
            capacity = new_capacity;
        }

        array[count++] = token;
    }

    close_scanner();

    *tokens = array;
    *token_count = count;

    return 1;
}

static int compile_beamer(void) {

    const char *command =
        "pdflatex "
        "-interaction=nonstopmode "
        "-halt-on-error "
        "-output-directory=presentation "
        "presentation/presentation.tex "
        ">" LATEX_LOG_FILENAME " 2>&1";

    if (system(command) != 0) {

        fprintf(
            stderr,
            "Error: pdflatex falló durante "
            "la primera compilación (ver %s)\n",
            LATEX_LOG_FILENAME
        );

        return 0;
    }

    if (system(command) != 0) {

        fprintf(
            stderr,
            "Error: pdflatex falló durante "
            "la segunda compilación (ver %s)\n",
            LATEX_LOG_FILENAME
        );

        return 0;
    }

    return 1;
}

static void show_pdf(void) {

    system(
        "evince --presentation "
        PDF_FILENAME
        " >/dev/null 2>&1 &"
    );
}


static void print_usage(const char *prog_name) {
    printf("Uso: %s [opciones] <archivo_fuente>\n\n", prog_name);
    printf("Opciones:\n");
    printf("  -h, --help       Muestra esta ayuda y termina\n");
    printf("  -v, --verbose    Muestra información detallada durante el proceso\n");
    printf("  -n, --no-view    Genera el PDF sin abrir evince automáticamente\n");
}

int main(
    int argc,
    char *argv[]
) {
    const char *source_filename = NULL;
    int show_view = 1;
    int verbose = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--no-view") == 0) {
            show_view = 0;
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Opción desconocida: %s\n", argv[i]);
            print_usage(argv[0]);
            return EXIT_FAILURE;
        } else {
            if (source_filename == NULL) {
                source_filename = argv[i];
            } else {
                fprintf(stderr, "Error: solo se puede especificar un archivo fuente\n");
                return EXIT_FAILURE;
            }
        }
    }

    if (source_filename == NULL) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    if (mkdir("presentation", 0755) == -1 &&
        errno != EEXIST) {

        fprintf(
            stderr,
            "Error: no se pudo crear "
            "el directorio presentation\n"
        );

        return EXIT_FAILURE;
    }

    /*
     * --------------------------------------------------
     * 1. PREPROCESSING
     * --------------------------------------------------
     */

    char temp_filename[PATH_MAX];

    int preprocessing_lexical_errors = 0;
    int preprocessing_errors = 0;

    if (!preprocess_file(
            source_filename,
            temp_filename,
            sizeof(temp_filename),
            &preprocessing_lexical_errors,
            &preprocessing_errors)) {

        fprintf(
            stderr,
            "Error fatal durante el preproceso\n"
        );

        return EXIT_FAILURE;
    }

    if (verbose) {
        printf("Archivo temporal generado: %s\n", temp_filename);
        printf("Errores léxicos de preproceso: %d\n", preprocessing_lexical_errors);
        printf("Errores de preprocesador: %d\n", preprocessing_errors);
    }

    /*
     * --------------------------------------------------
     * 2. SCANNING
     * --------------------------------------------------
     */

    Token *tokens = NULL;
    size_t token_count = 0;

    if (!scan_file(
            temp_filename,
            &tokens,
            &token_count)) {

        fprintf(
            stderr,
            "Error durante el scanning\n"
        );

        return EXIT_FAILURE;
    }

    if (verbose) {
        printf("Tokens encontrados: %zu\n", token_count);
    }

    /*
     * --------------------------------------------------
     * 3. BEAMER GENERATION
     * --------------------------------------------------
     */

    if (!beamer_generate(
            TEX_FILENAME,
            source_filename,
            temp_filename,
            tokens,
            token_count)) {

        fprintf(
            stderr,
            "Error generando la presentación Beamer\n"
        );

        free_tokens(
            tokens,
            token_count
        );

        return EXIT_FAILURE;
    }

    free_tokens(
        tokens,
        token_count
    );

    /*
     * --------------------------------------------------
     * 4. PDF GENERATION
     * --------------------------------------------------
     */

    if (!compile_beamer()) {

        return EXIT_FAILURE;
    }

    /*
     * --------------------------------------------------
     * 5. PDF VISUALIZATION
     * --------------------------------------------------
     */

    if (show_view) {
        show_pdf();
    }

    if (verbose) {
        printf("Presentación generada exitosamente: %s\n", PDF_FILENAME);
    }

    return EXIT_SUCCESS;
}