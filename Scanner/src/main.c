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


/*
 * Releases every lexeme allocated by the scanner and
 * then releases the token array itself.
 */
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


/*
 * Runs the lexical scanner and stores every token
 * dynamically in memory.
 */
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

        /*
         * EOF is useful to stop scanning but does not need
         * to be stored for the presentation.
         */
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


/*
 * Compiles the generated Beamer source.
 */
static int compile_beamer(void) {

    const char *command =
        "pdflatex "
        "-interaction=nonstopmode "
        "-halt-on-error "
        "-output-directory=presentation "
        "presentation/presentation.tex "
        ">" LATEX_LOG_FILENAME " 2>&1";

    /*
     * Beamer is compiled twice so navigation information
     * and auxiliary data are generated correctly.
     */

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


/*
 * Opens the generated PDF using Evince in presentation mode.
 */
static void show_pdf(void) {

    system(
        "evince --presentation "
        PDF_FILENAME
        " >/dev/null 2>&1 &"
    );
}


int main(
    int argc,
    char *argv[]
) {
    if (argc != 2) {

        fprintf(
            stderr,
            "Uso: %s <archivo_fuente>\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    /*
     * Ensure the output directory exists.
     */
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
            argv[1],
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

    printf(
        "Archivo temporal generado: %s\n",
        temp_filename
    );

    printf(
        "Errores léxicos de preproceso: %d\n",
        preprocessing_lexical_errors
    );

    printf(
        "Errores de preprocesador: %d\n",
        preprocessing_errors
    );


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

    printf(
        "Tokens encontrados: %zu\n",
        token_count
    );


    /*
     * --------------------------------------------------
     * 3. BEAMER GENERATION
     * --------------------------------------------------
     */

    if (!beamer_generate(
            TEX_FILENAME,
            argv[1],
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


    /*
     * Tokens are no longer necessary after the .tex
     * source has been generated.
     */
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

    show_pdf();

    printf(
        "Presentación generada: %s\n",
        PDF_FILENAME
    );

    return EXIT_SUCCESS;
}