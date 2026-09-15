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

        char key_buffer[MAX_KEY];
        char value_buffer[MAX_VALUE];

        int key_index = 0;
        int value_index = 0;

        while (*temp == ' ' || *temp == '\t') temp++;

        if (*temp == '#') temp++;

        while (*temp == ' ' || *temp == '\t') temp++;

        if (*temp == 'd') temp += 6;

        while (*temp == ' ' || *temp == '\t') temp++;

        while (*temp == '_' || isalnum((unsigned char)*temp)) {
            key_buffer[key_index] = *temp;
            key_index++;
            temp++;
        }

        key_buffer[key_index] = '\0';

        while (*temp == ' ' || *temp == '\t') temp++;

        while (*temp != '\0' && value_index < MAX_VALUE - 1) {

            if (temp[0] == '/' && temp[1] == '/') {
                break;
            }

            if (temp[0] == '/' && temp[1] == '*') {
                temp += 2;

                while (*temp != '\0' &&
                    !(temp[0] == '*' && temp[1] == '/')) {
                    temp++;
                }

                if (*temp != '\0') {
                    temp += 2;
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

        while (value_index > 0 && (value_buffer[value_index - 1] == ' '  ||
                                   value_buffer[value_index - 1] == '\t' ||
                                   value_buffer[value_index - 1] == '\r')) {
            value_index--;
        }

        value_buffer[value_index] = '\0';

        MacroType type = macro_type(value_buffer);

        return macro_insert(key_buffer, value_buffer, type);
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

                char identifier[MAX_KEY];
                size_t id_index = 0;

                while (value[i] == '_' ||
                    isalnum((unsigned char)value[i])) {

                    if (id_index < MAX_KEY - 1) {
                        identifier[id_index++] = value[i];
                    }

                    i++;
                }

                identifier[id_index] = '\0';

                const char* replacement = macro_find(identifier);

                if (replacement != NULL) {

                    char expanded[MAX_VALUE];

                    if (!expand_macro_value(
                            replacement,
                            expanded,
                            sizeof(expanded),
                            depth + 1)) {
                        return 0;
                    }

                    size_t length = strlen(expanded);

                    if (out_index + length >= output_size) {
                        fprintf(stderr, "Error: expansión de macro demasiado larga\n");
                        preprocessor_error_count++;
                        return 0;
                    }

                    memcpy(output + out_index, expanded, length);
                    out_index += length;

                } else {

                    size_t length = strlen(identifier);

                    if (out_index + length >= output_size) {
                        fprintf(stderr, "Error: expansión de macro demasiado larga\n");
                        preprocessor_error_count++;
                        return 0;
                    }

                    memcpy(output + out_index, identifier, length);
                    out_index += length;
                }

            } else {

                if (out_index + 1 >= output_size) {
                    fprintf(stderr, "Error: expansión de macro demasiado larga\n");
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
%x COMMENT

/*
 * Rules:
 *
 * 1. Recognizes #define directives and sends the complete line
 *    to macro_handle_define().
 *
 * 2. Recognizes #include directives, extracts the filename,
 *    opens the included file and pushes a new Flex buffer so
 *    the included file is processed recursively.
 *
 * 3. Recognizes string literals and copies them without attempting
 *    to expand macros inside the string.
 *
 * 4. Recognizes character literals and copies them without attempting
 *    to expand macros inside the character literal.
 *
 * 5. Recognizes identifiers. If the identifier corresponds to a stored
 *    macro, its value is recursively expanded; otherwise it is copied.
 *
 * 6. Recognizes // comments and discards them.
 *
 * 7. Recognizes the beginning of a block comment and enters
 *    the COMMENT state.
 *
 * 8. Recognizes the end of a block comment and returns to
 *    the INITIAL state.
 *
 * 9. While inside COMMENT, newlines and other characters are discarded.
 *
 * 10. If EOF is reached while still inside COMMENT, an unclosed
 *     comment lexical error is reported.
 *
 * 11. When EOF is reached in an included file, the current file
 *     is closed and Flex returns to the previous input buffer.
 *     If there are no pending includes, preprocessing finishes.
 */

%%
^[ \t]*#[ \t]*define[ \t]+[A-Za-z_][A-Za-z0-9_]*[^\n]* {
    macro_handle_define(yytext);
}

^[ \t]*#[ \t]*include[ \t]+(\"[^\"\n]+\"|<[^>\n]+>)[ \t]* {
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
}

\"([^\\\n]|\\.)*\"    {
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

"/*"                { BEGIN(COMMENT); }
<COMMENT>"*/"       { BEGIN(INITIAL); }
<COMMENT>\n         {;}
<COMMENT>.          {;}

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
int main(){

    // Explanation:
    // yywrap() - wraps the above rule section
    /* yyin - takes the file pointer 
            which contains the input*/
    /* yylex() - this is the main flex function
            which runs the Rule Section*/
    // yytext is the text in the buffer

    // Uncomment the lines below 
    // to take input from file
    FILE *fp;
    char filename[50];

    printf("Enter the filename: \n");
    scanf("%s", filename);

    fp = fopen(filename, "r");

    if (fp == NULL) {
        fprintf(stderr, "Error: no se pudo abrir %s\n", filename);
        return 1;
    }

    char temp_filename[] = "/tmp/preprocessor_XXXXXX";

    int temp_fd = mkstemp(temp_filename);

    if (temp_fd == -1) {
        fprintf(stderr, "Error: no se pudo crear archivo temporal\n");
        fclose(fp);
        return 1;
    }

    FILE *temp_file = fdopen(temp_fd, "w");

    if (temp_file == NULL) {
        fprintf(stderr, "Error: no se pudo abrir archivo temporal\n");
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

    // print_macros();

    printf("\nErrores lexicos: %d\n", lexical_error_count);
    printf("Errores de preprocesador: %d\n", preprocessor_error_count);

    return 0;
}