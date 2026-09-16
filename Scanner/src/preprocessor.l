%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <unistd.h>

    #define MAX_MACROS 1000
    #define MAX_KEY 128
    #define MAX_VALUE 1024
    #define MAX_DEPTH 32
    #define MAX_TYPES 8

    typedef enum {
        M_INT, //
        M_DOUBLE, //
        M_STRING, //
        M_CHAR, //
        M_IDENT,
        M_EXPR,
        M_EXPANDED,
        M_EMPTY //
    } MacroType;

    static const char* TYPE_NAME[MAX_TYPES] = 
    {"int", "double", "string", "char", "ident", "expr", "expanded", "empty"};

    typedef struct {
        char* key;
        char* value;
        MacroType type;
    } Macro;

    static Macro macro_table[MAX_MACROS];
    static int macro_count = 0;
    
    // Counters for lexical and preprocessor errors.
    
    static int lexical_error_count = 0;
    static int preprocessor_error_count = 0;

    // State variable to track whether we are inside a comment or not.
    static int comment_return_state = 0;

   /*
    * Stack of files currently being processed through #include.
    * include_depth tracks the current nesting level.
    */
    static FILE* include_files[MAX_DEPTH];
    static int include_depth = 0;

    /*
     * Prints all macros currently stored in the macro table.
     * Used for debugging and verification of the stored key,
     * value and detected macro type.
     */
    static void print_macros() {
        for (int i = 0; i < macro_count; i++) {
            printf("\n%s\n%s\n%d\n", macro_table[i].key,macro_table[i].value, macro_table[i].type );
        }
    }

    /*
     * Inserts a new macro into the macro table.
     * Checks that the table is not full and that the macro
     * has not already been defined before storing it.
     */
    static int macro_insert(const char* key, const char* value, const MacroType type) {
        if (macro_count >= MAX_MACROS) {
          fprintf(stderr, "Error: tabla de macros llena\n");
          preprocessor_error_count++;
          return 0;
      }
        
        for (int i = 0; i < macro_count; i++) {
            if (strcmp(macro_table[i].key, key) == 0) {
                fprintf(stderr, "Error: Macro %s ya definida\n", key);
                preprocessor_error_count++;
                return 0;
            }
        }

        macro_table[macro_count].key = strdup(key);
        macro_table[macro_count].value = strdup(value);
        macro_table[macro_count].type = type;

        macro_count++;

        return 1;
    }

    /*
     * Determines the type of value associated with a macro.
     * Values are classified as empty, string, char, integer,
     * double, identifier or expression.
     */
    static MacroType macro_type(const char* value) {
        const char* valuePointer = value;

        if (valuePointer[0] == '\0') return M_EMPTY;
        if (valuePointer[0] == '"') return M_STRING;
        if (valuePointer[0] == '\'') return M_CHAR;

        char* pEnd;

        strtol(valuePointer, &pEnd, 0);
        if (valuePointer != pEnd && *pEnd == '\0') return M_INT;

        strtod(valuePointer, &pEnd);
        if (valuePointer != pEnd && *pEnd == '\0') return M_DOUBLE;

        if (valuePointer[0] == '_' || isalpha((unsigned char)valuePointer[0])) {
            while (*valuePointer == '_' || isalnum((unsigned char)*valuePointer)) valuePointer++;
            if (*valuePointer == '\0') return M_IDENT;
        }

        return M_EXPR;
    }

    /*
     * Searches for a macro by its key in the macro table.
     * Returns the associated value if found, or NULL if the
     * identifier has not been defined as a macro.
     */
    static const char* macro_find(const char* key) {
        for (int i = 0; i < macro_count; i++) {
            if (strcmp(macro_table[i].key, key) == 0) {
                return macro_table[i].value;
            }
        }

        return NULL;
    }

        /*
    * Processes the complete text of a #define directive.
    * Extracts the macro name and its associated value,
    * determines its type and stores it in the macro table.
    */
    static int macro_handle_define(const char* text) {
        const char* temp = text;

        char value_buffer[MAX_VALUE];
        int value_index = 0;

        while (*temp == ' ' || *temp == '\t') temp++;

        if (*temp == '#') temp++;

        while (*temp == ' ' || *temp == '\t') temp++;

        if (*temp == 'd') temp += 6;

        while (*temp == ' ' || *temp == '\t') temp++;

        /*
        * Save where the macro name begins and advance until
        * the complete identifier has been read.
        */
        const char* key_start = temp;

        while (*temp == '_' || isalnum((unsigned char)*temp)) {
            temp++;
        }

        size_t key_length = (size_t)(temp - key_start);

        /*
        * Allocate exactly enough memory for the macro name.
        */
        char* key_buffer = malloc(key_length + 1);

        if (key_buffer == NULL) {
            fprintf(stderr, "Error: no se pudo reservar memoria para el macro\n");
            preprocessor_error_count++;
            return 0;
        }

        memcpy(key_buffer, key_start, key_length);
        key_buffer[key_length] = '\0';

        while (*temp == ' ' || *temp == '\t') temp++;

        while (*temp != '\0' && value_index < MAX_VALUE - 1) {

            if (temp[0] == '/' && temp[1] == '*') {
                temp += 2;

                while (*temp != '\0' &&
                    !(temp[0] == '*' && temp[1] == '/')) {
                    temp++;
                }

                if (*temp != '\0') {
                    temp += 2;
                }

                /* A comment in C behaves like whitespace */
                if (value_index > 0 &&
                    value_index < MAX_VALUE - 1 &&
                    value_buffer[value_index - 1] != ' ') {

                    value_buffer[value_index++] = ' ';
                }

                continue;
            }

            if (*temp == '"' || *temp == '\'') {
                char quote = *temp;

                value_buffer[value_index++] = *temp++;

                while (*temp != '\0' && value_index < MAX_VALUE - 1) {
                    value_buffer[value_index++] = *temp;

                    if (*temp == '\\' && temp[1] != '\0') {
                        temp++;

                        if (value_index < MAX_VALUE - 1) {
                            value_buffer[value_index++] = *temp++;
                        }

                        continue;
                    }

                    if (*temp == quote) {
                        temp++;
                        break;
                    }

                    temp++;
                }

                continue;
            }

            value_buffer[value_index++] = *temp++;
        }

        while (value_index > 0 &&
            (value_buffer[value_index - 1] == ' '  ||
                value_buffer[value_index - 1] == '\t' ||
                value_buffer[value_index - 1] == '\r')) {

            value_index--;
        }

        value_buffer[value_index] = '\0';

        MacroType type = macro_type(value_buffer);

        int result = macro_insert(key_buffer, value_buffer, type);

        free(key_buffer);

        return result;
    }

    /*
    * Extracts the filename from an #include directive.
    * Supports both "file" and <file> syntax.
    */
    static int include_get_filename(const char* text, char* filename, size_t size) {
        const char* start = strchr(text, '"');
        char end = '"';

        if (start == NULL) {
            start = strchr(text, '<');
            end = '>';
        }

        if (start == NULL) {
            return 0;
        }

        start++;

        const char* finish = strchr(start, end);

        if (finish == NULL) {
            return 0;
        }

        size_t length = finish - start;

        if (length >= size) {
            return 0;
        }

        memcpy(filename, start, length);
        filename[length] = '\0';

        return 1;
    }

    /*
     * Recursively expands macros contained inside another macro value.
     * Identifiers are searched in the macro table and replaced by their
     * values. Strings and character literals are copied without expanding
     * their internal contents. MAX_DEPTH prevents infinite recursion.
     */
    static int expand_macro_value(const char* value, char* output,
                              size_t output_size, int depth) {
        if (depth >= MAX_DEPTH) {
            fprintf(stderr, "Error: profundidad maxima de macros alcanzada\n");
            preprocessor_error_count++;
            return 0;
        }

        size_t i = 0;
        size_t out_index = 0;

        while (value[i] != '\0') {
            if (value[i] == '"' || value[i] == '\'') {
                char quote = value[i];

                if (out_index + 1 >= output_size) {
                    return 0;
                }

                output[out_index++] = value[i++];

                while (value[i] != '\0') {

                    if (out_index + 1 >= output_size) {
                        return 0;
                    }

                    output[out_index++] = value[i];

                    // Carácter escapado: \"  \'  \\  \n ...
                    if (value[i] == '\\' && value[i + 1] != '\0') {
                        i++;

                        if (out_index + 1 >= output_size) {
                            return 0;
                        }

                        output[out_index++] = value[i++];
                        continue;
                    }

                    if (value[i] == quote) {
                        i++;
                        break;
                    }

                    i++;
                }

                continue;
            }
            if (value[i] == '_' || isalpha((unsigned char)value[i])) {

                /*
                * Save the beginning of the identifier and advance
                * until the complete identifier has been read.
                */
                size_t id_start = i;

                while (value[i] == '_' ||
                    isalnum((unsigned char)value[i])) {
                    i++;
                }

                size_t id_length = i - id_start;

                /*
                * Allocate exactly enough memory for the identifier.
                */
                char *identifier = malloc(id_length + 1);

                if (identifier == NULL) {
                    fprintf(stderr,
                            "Error: no se pudo reservar memoria para el identificador\n");
                    preprocessor_error_count++;
                    return 0;
                }

                memcpy(identifier, value + id_start, id_length);
                identifier[id_length] = '\0';

                const char *replacement = macro_find(identifier);

                if (replacement != NULL) {

                    char expanded[MAX_VALUE];

                    if (!expand_macro_value(
                            replacement,
                            expanded,
                            sizeof(expanded),
                            depth + 1)) {

                        free(identifier);
                        return 0;
                    }

                    size_t length = strlen(expanded);

                    if (out_index + length >= output_size) {
                        fprintf(stderr,
                                "Error: expansión de macro demasiado larga\n");

                        preprocessor_error_count++;
                        free(identifier);
                        return 0;
                    }

                    memcpy(output + out_index, expanded, length);
                    out_index += length;

                } else {

                    if (out_index + id_length >= output_size) {
                        fprintf(stderr,
                                "Error: expansión de macro demasiado larga\n");

                        preprocessor_error_count++;
                        free(identifier);
                        return 0;
                    }

                    memcpy(output + out_index, identifier, id_length);
                    out_index += id_length;
                }

                free(identifier);

            } else {

                if (out_index + 1 >= output_size) {
                    fprintf(stderr,
                            "Error: expansión de macro demasiado larga\n");

                    preprocessor_error_count++;
                    return 0;
                }

                output[out_index++] = value[i++];
            }
        }

        output[out_index] = '\0';

        return 1;
    }
%}

/* State for multiline comments */
%x COMMENT INCLUDE

/*
 * Preprocessor lexical rules:
 *
 * 1. Recognizes #define directives containing a block comment
 *    on the same line and sends the complete directive to
 *    macro_handle_define().
 *
 * 2. Recognizes regular #define directives and sends the
 *    complete line to macro_handle_define().
 *
 * 3. Detects an incomplete #include directive followed by
 *    a newline and reports a preprocessor error.
 *
 * 4. Recognizes an #include directive immediately followed
 *    by a block comment and enters the COMMENT state.
 *
 * 5. Recognizes an #include directive followed by whitespace
 *    and enters the INCLUDE state.
 *
 * 6. Acts as a fallback for #include directives without
 *    whitespace, including cases such as #include"file.h",
 *    #include<file.h>, or #include followed directly by EOF.
 *
 * 7. Ignores whitespace while processing an #include directive.
 *
 * 8. Detects a newline while still waiting for an include
 *    filename and reports an incomplete #include directive.
 *
 * 9. Recognizes a block comment while processing an #include
 *    directive and temporarily enters the COMMENT state.
 *
 * 10. Recognizes the filename of an #include directive,
 *     opens the file, and pushes a new Flex buffer so that
 *     the included file is processed recursively.
 *
 * 11. Recognizes string literals and copies them to the
 *     output without expanding macros inside them.
 *
 * 12. Recognizes character literals and copies them to the
 *     output without expanding macros inside them.
 *
 * 13. Recognizes identifiers. If an identifier corresponds
 *     to a defined macro, its value is recursively expanded;
 *     otherwise, the identifier is copied unchanged.
 *
 * 14. Recognizes // comments and discards their contents.
 *
 * 15. Recognizes the beginning of a block comment in normal
 *     source code, writes whitespace to prevent adjacent
 *     tokens from joining, and enters the COMMENT state.
 *
 * 16. Recognizes the end of a block comment and returns to
 *     the lexical state active before the comment began.
 *
 * 17. Preserves newline characters contained in block comments
 *     when the comment originated in normal source code.
 *
 * 18. Discards all other characters while inside a block comment.
 *
 * 19. Detects EOF while inside a block comment, reports an
 *     unclosed-comment lexical error, and returns to the
 *     parent included file when necessary.
 *
 * 20. Detects EOF while waiting for an #include filename,
 *     reports an incomplete-directive error, and returns to
 *     the parent included file when necessary.
 *
 * 21. Handles normal EOF. If the current file was included,
 *     it closes that file and restores the previous Flex buffer.
 *     If there are no pending includes, preprocessing finishes.
 */

%%
^[ \t]*#[ \t]*define[ \t]+[A-Za-z_][A-Za-z0-9_]*[^\n]*"/*"([^*]|\*+[^*/])*\*+"/"[^\n]* {
    macro_handle_define(yytext);
}

^[ \t]*#[ \t]*define[ \t]+[A-Za-z_][A-Za-z0-9_]*[^\n]* {
    macro_handle_define(yytext);
}

^[ \t]*#[ \t]*include[ \t]*\n {
    fprintf(stderr, "Error: directiva include incompleta\n");
    preprocessor_error_count++;
}

^[ \t]*#[ \t]*include"/*" {
    comment_return_state = INCLUDE;
    BEGIN(COMMENT);
}

^[ \t]*#[ \t]*include[ \t]+ {
    BEGIN(INCLUDE);
}

    /*
    * Fallback for cases such as:
    *
    * #include
    *
    * followed immediately by EOF.
    *
    * It also allows forms such as:
    * #include"file.h"
    * #include<file.h>
    */
^[ \t]*#[ \t]*include {
    BEGIN(INCLUDE);
}

<INCLUDE>[ \t]+ {;}

<INCLUDE>\n {
    fprintf(stderr, "Error: directiva include incompleta\n");
    preprocessor_error_count++;
    BEGIN(INITIAL);
}

<INCLUDE>"/*" {
    comment_return_state = YY_START;
    BEGIN(COMMENT);
}

<INCLUDE>(\"[^\"\n]+\"|<[^>\n]+>) {
    char filename[MAX_VALUE];

    if (include_get_filename(yytext, filename, sizeof(filename))) {
        FILE* included_file = fopen(filename, "r");

        if (included_file == NULL) {
            fprintf(stderr, "Error: no se pudo abrir %s\n", filename);
            preprocessor_error_count++;
        } else if (include_depth >= MAX_DEPTH) {
            fprintf(stderr, "Error: demasiados includes anidados\n");
            preprocessor_error_count++;
            fclose(included_file);
        } else {
            include_files[include_depth++] = included_file;

            yypush_buffer_state(
                yy_create_buffer(included_file, YY_BUF_SIZE)
            );
        }
    }

    BEGIN(INITIAL);
}

\"([^\\\n]|\\.)*\" {
    fprintf(yyout, "%s", yytext);
}

\'([^\\'\n]|\\.)+\' {
    fprintf(yyout, "%s", yytext);
}

[A-Za-z_][A-Za-z0-9_]* {
    const char* value = macro_find(yytext);

    if (value != NULL) {
        char expanded[MAX_VALUE];

        if (expand_macro_value(value, expanded, sizeof(expanded), 0)) {
            fprintf(yyout, "%s", expanded);
        } else {
            fprintf(yyout, "%s", yytext);
        }
    } else {
        fprintf(yyout, "%s", yytext);
    }
}

"//"[^\n]*          {;}

"/\*" {
    comment_return_state = YY_START;

    /*
     * A block comment behaves as whitespace.
     * This prevents tokens around the comment from
     * being joined together.
     */
    fputc(' ', yyout);

    BEGIN(COMMENT);
}

<COMMENT>"*/" {
    BEGIN(comment_return_state);
}
<COMMENT>\n {
    /*
     * Preserve newlines only when the comment came from
     * normal source code.
     *
     * Comments inside an #include directive must not be
     * copied to the preprocessed output.
     */
    if (comment_return_state == INITIAL) {
        fputc('\n', yyout);
    }
}
<COMMENT>. {;}

<COMMENT><<EOF>> {
    fprintf(stderr, "Error léxico: comentario sin cerrar\n");
    lexical_error_count++;

    BEGIN(INITIAL);

    if (include_depth > 0) {
        fclose(include_files[--include_depth]);
        yypop_buffer_state();
    } else {
        return 0;
    }
}

<INCLUDE><<EOF>> {
    fprintf(stderr, "Error: directiva include incompleta\n");
    preprocessor_error_count++;

    BEGIN(INITIAL);

    /*
     * If the malformed #include was inside an included file,
     * close it and continue with its parent.
     */
    if (include_depth > 0) {

        fclose(include_files[--include_depth]);
        yypop_buffer_state();

    } else {

        return 0;
    }
}

<INITIAL><<EOF>> {
    if (include_depth > 0) {
        fclose(include_files[--include_depth]);
        yypop_buffer_state();
    } else {
        return 0;
    }
}
%%

/*
 * Indicates to Flex that there are no additional input files
 * after reaching the end of the current input.
 */
int yywrap(){
    return 1;
}

/*
 * Opens the input file, assigns it to Flex through yyin,
 * executes the lexical preprocessing with yylex(), closes
 * the file and prints the macro table for verification.
 */
int main(int argc, char *argv[]){

    // Explanation:
    // yywrap() - wraps the above rule section
    /* yyin - takes the file pointer 
            which contains the input*/
    /* yylex() - this is the main flex function
            which runs the Rule Section*/
    // yytext is the text in the buffer

    // Uncomment the lines below 
    // to take input from file
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.c>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");

    if (fp == NULL) {
        fprintf(stderr, "Error: no se pudo abrir %s\n", argv[1]);
        return 1;
    }

    char temp_filename[] = "/tmp/preprocessor_XXXXXX";

    int temp_fd = mkstemp(temp_filename);

    if (temp_fd == -1) {
        fprintf(stderr, "Error: no se pudo crear el archivo temporal\n");
        fclose(fp);
        return 1;
    }

    FILE *temp_file = fdopen(temp_fd, "w");

    if (temp_file == NULL) {
        fprintf(stderr, "Error: no se pudo abrir el archivo temporal\n");
        close(temp_fd);
        fclose(fp);
        return 1;
    }

    yyin = fp;
    yyout = temp_file;

    yylex();

    fclose(fp);
    fclose(temp_file);

    printf("Archivo temporal generado: %s\n", temp_filename);
    printf("Errores lexicos: %d\n", lexical_error_count);
    printf("Errores de preprocesador: %d\n", preprocessor_error_count);

    return 0;
}