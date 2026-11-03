#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <getopt.h>

#include "preprocessor.h"
#include "parser.h"

#define VERSION "1.0.0"

// Help commands
static void print_usage(const char *prog_name) {
    fprintf(stderr, "Uso: %s [OPCIONES] <archivo_fuente>\n", prog_name);
    fprintf(stderr, "\nOpciones disponibles:\n");
    fprintf(stderr, "  -h, --help       Muestra este menú de ayuda y termina.\n");
    fprintf(stderr, "  -v, --version    Muestra la versión del programa.\n");
    fprintf(stderr, "  -s, --stats      Muestra el resumen de errores encontrados.\n");
    fprintf(stderr, "\nArgumentos:\n");
    fprintf(stderr, "  <archivo_fuente> Archivo en lenguaje C a procesar (cualquier extensión).\n");
    fprintf(stderr, "\nEjemplos:\n");
    fprintf(stderr, "  %s programa.c\n", prog_name);
    fprintf(stderr, "  %s -s prueba.txt\n", prog_name);
}

static void print_version(void) {
    printf("C Parser (Proyecto 2 - Compiladores) versión %s\n", VERSION);
}

int main(int argc, char *argv[]) {
    // if not args: "./bin/parser"
    // display usage
    if (argc < 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int opt;
    int show_stats = 0;

    static struct option long_options[] = {
        {"help",    no_argument, 0, 'h'},
        {"version", no_argument, 0, 'v'},
        {"stats",   no_argument, 0, 's'},
        {0, 0, 0, 0}
    };

    while ((opt = getopt_long(argc, argv, "hvs", long_options, NULL)) != -1) {
        switch (opt) {
            case 'h':
                print_usage(argv[0]);
                return EXIT_SUCCESS;
            case 'v':
                print_version();
                return EXIT_SUCCESS;
            case 's':
                show_stats = 1;
                break;
            default:
                // Unkonwn option
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Verify the input file
    if (optind >= argc) {
        fprintf(stderr, "Error: Debe indicar un archivo fuente para procesar.\n\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    // Verify not unexpected args
    if (argc - optind > 1) {
        fprintf(stderr, "Error: Solo se permite procesar un archivo a la vez.\n\n");
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_filename = argv[optind];

    // Verify the files existance before try to compile
    if (access(input_filename, R_OK) != 0) {
        fprintf(stderr, "Error: No se pudo abrir o leer el archivo '%s'\n", input_filename);
        return EXIT_FAILURE;
    }

    // 1. Preprocess
    char temp_filename[PATH_MAX];
    int lexical_errors = 0;
    int preprocessor_errors = 0;

    if (!preprocess_file(
            input_filename,
            temp_filename,
            sizeof(temp_filename),
            &lexical_errors,
            &preprocessor_errors)) {
        fprintf(stderr, "Error fatal durante el preproceso\n");
        return EXIT_FAILURE;
    }

    // 2. 
    // ------------------------ parse the file --------------------------
    int syntax_errors = 0;
    int ok = parse_file(temp_filename, &syntax_errors);
    // ------------------------ parse the file --------------------------

    // 3. Delete the temp file
    unlink(temp_filename);

    // Statics on arg "-s"
    if (show_stats) {
        printf("Errores léxicos de preproceso: %d\n", lexical_errors);
        printf("Errores de preprocesador: %d\n", preprocessor_errors);
        printf("Errores de sintaxis: %d\n", syntax_errors);
    }

    // The program fails on any lex, preprocess or syntax error
    int total_errors = lexical_errors + preprocessor_errors + syntax_errors;
    return (ok && total_errors == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}