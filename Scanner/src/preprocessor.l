%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include <unistd.h>
    #include <stdbool.h>
    #include <limits.h>

    #include "preprocessor.h"

    #define MAX_MACROS 1000
    #define MAX_KEY 128
    #define MAX_VALUE 1024
    #define MAX_DEPTH 32
    #define MAX_ARGS 32

    typedef enum {
        M_INT,
        M_DOUBLE,
        M_STRING,
        M_CHAR,
        M_IDENT,
        M_EXPR,
        M_EXPANDED,
        M_FUNCTION,
        M_EMPTY
    } MacroType;

    typedef struct {
        char* key;
        char* value;
        MacroType type;
        int arg_count;
        char* args[MAX_ARGS];
    } Macro;

    static Macro macro_table[MAX_MACROS];
    static int macro_count = 0;
    
    static int lexical_error_count = 0;
    static int preprocessor_error_count = 0;
    static int definitive_error = 1;

    static int comment_return_state = 0;

    static FILE* include_files[MAX_DEPTH];
    static int include_depth = 0;
    static char include_dirs[MAX_DEPTH + 1][PATH_MAX];

    static void free_macro_table(void) {
        for (int i = 0; i < macro_count; i++) {
            free(macro_table[i].key);
            free(macro_table[i].value);
            for (int j = 0; j < macro_table[i].arg_count; j++) {
                free(macro_table[i].args[j]);
            }
        }
        macro_count = 0;
    }

    static void get_dirname(const char* filepath, char* dir, size_t dir_size) {
        const char* last_slash = strrchr(filepath, '/');
        if (last_slash != NULL) {
            size_t len = (size_t)(last_slash - filepath);
            if (len >= dir_size) len = dir_size - 1;
            memcpy(dir, filepath, len);
            dir[len] = '\0';
        } else {
            strcpy(dir, ".");
        }
    }

    static size_t skip_comment(const char* text, size_t i) {
        if (text[i] == '/' && text[i + 1] == '*') {
            i += 2;
            while (text[i] != '\0' && !(text[i] == '*' && text[i + 1] == '/')) {
                i++;
            }
            if (text[i] == '*' && text[i + 1] == '/') {
                i += 2;
            }
            return i;
        }
        if (text[i] == '/' && text[i + 1] == '/') {
            i += 2;
            while (text[i] != '\0' && text[i] != '\n') {
                i++;
            }
            return i;
        }
        return i;
    }

    static int expand_macro_value(const char* value, char* output,
                              size_t output_size, int depth,
                              const Macro** pending);

    static void handle_function_call(const Macro* macro, const char* name);

    static int macro_insert(const char* key, const char* value, const MacroType type,
                            char** args, int arg_count) {
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

        macro_table[macro_count].arg_count = arg_count;

        for (int i = 0; i < arg_count; i++) {
            macro_table[macro_count].args[i] = strdup(args[i]);
        }

        macro_count++;

        return 1;
    }

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

    static const Macro* macro_find(const char* key) {
        for (int i = 0; i < macro_count; i++) {
            if (strcmp(macro_table[i].key, key) == 0) {
                return &macro_table[i];
            }
        }

        return NULL;
    }

    static void free_args(char** args, int count) {
        for (int i = 0; i < count; i++) {
            free(args[i]);
        }
    }

    static int define_error(char* key, char** args, int count, const char* message) {
        fprintf(stderr, "Error: %s para %s\n", message, key);
        preprocessor_error_count++;
        definitive_error = 0;
        free(key);
        free_args(args, count);
        return 0;
    }

    static int is_ident_start(int c) {
        return c == '_' || isalpha((unsigned char)c);
    }

    static int is_ident_char(int c) {
        return c == '_' || isalnum((unsigned char)c);
    }

    static int only_blanks_left(const char* text) {
        while (*text == ' '  || *text == '\t' ||
               *text == '\r' || *text == '\n') {
            text++;
        }

        return *text == '\0';
    }

    
    static size_t continuation_length(const char* temp) {
        if (temp[0] != '\\') {
            return 0;
        }

        size_t length = 1;

        while (temp[length] == ' '  ||
               temp[length] == '\t' ||
               temp[length] == '\r') {
            length++;
        }

        if (temp[length] != '\n') {
            return 0;
        }

        return length + 1;
    }

    static const char* skip_blanks(const char* temp) {
        while (1) {
            if (*temp == ' ' || *temp == '\t' || *temp == '\r') {
                temp++;
                continue;
            }

            size_t continuation = continuation_length(temp);

            if (continuation == 0) {
                return temp;
            }

            temp += continuation;
        }
    }

    static void keep_line_count(const char* text) {
        for (const char* temp = text; *temp != '\0'; temp++) {
            if (*temp == '\n') {
                fputc('\n', yyout);
            }
        }
    }

    static int macro_handle_define(const char* text) {
        const char* temp = text;

        char value_buffer[MAX_VALUE];
        int value_index = 0;

        temp = skip_blanks(temp);

        if (*temp == '#') temp++;

        temp = skip_blanks(temp);

        if (*temp == 'd') temp += 6;

        temp = skip_blanks(temp);

        const char* key_start = temp;

        if (*temp != '_' && !isalpha((unsigned char)*temp)) {
            lexical_error_count++;
            fprintf(stderr, "Error: nombre de macro invalido\n");
            return 0;
        }

        while (*temp == '_' || isalnum((unsigned char)*temp)) {
            temp++;
        }

        size_t key_length = (size_t)(temp - key_start);

        char* key_buffer = malloc(key_length + 1);

        if (key_buffer == NULL) {
            fprintf(stderr, "Error: no se pudo reservar memoria para el macro\n");
            definitive_error = 0;
            preprocessor_error_count++;
            return 0;
        }

        memcpy(key_buffer, key_start, key_length);
        key_buffer[key_length] = '\0';

        char* params[MAX_ARGS];
        int param_count = 0;
        bool is_function = false;

        if (*temp == '(') {
            is_function = true;
            temp++;
            temp = skip_blanks(temp);

            if (*temp != ')') {
                while (true) {
                    temp = skip_blanks(temp);

                    if (!is_ident_start(*temp)) {
                        return define_error(key_buffer, params, param_count,
                                            "parametro de macro invalido");
                    }

                    const char* param_start = temp;

                    while (is_ident_char(*temp)) {
                        temp++;
                    }

                    size_t param_length = (size_t)(temp - param_start);

                    if (param_count >= MAX_ARGS) {
                        return define_error(key_buffer, params, param_count,
                                            "demasiados parametros en la macro");
                    }

                    for (int i = 0; i < param_count; i++) {
                        if (strlen(params[i]) == param_length &&
                            strncmp(params[i], param_start, param_length) == 0) {
                            return define_error(key_buffer, params, param_count,
                                                "parametro de macro duplicado");
                        }
                    }

                    params[param_count] = malloc(param_length + 1);

                    if (params[param_count] == NULL) {
                        return define_error(key_buffer, params, param_count,
                                            "no se pudo reservar memoria para el macro");
                    }

                    memcpy(params[param_count], param_start, param_length);
                    params[param_count][param_length] = '\0';
                    param_count++;

                    temp = skip_blanks(temp);

                    if (*temp == ',') {
                        temp++;
                        continue;
                    }

                    break;
                }
            }

            if (*temp != ')') {
                return define_error(key_buffer, params, param_count,
                                    "lista de parametros de macro sin cerrar");
            }

            temp++;
        }

        temp = skip_blanks(temp);

        while (*temp != '\0' && value_index < MAX_VALUE - 1) {

            size_t continuation = continuation_length(temp);

            if (continuation > 0) {
                temp += continuation;
                continue;
            }

            if (*temp == '\n') {
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

                if (value_index > 0 &&
                    value_index < MAX_VALUE - 1 &&
                    value_buffer[value_index - 1] != ' ') {

                    value_buffer[value_index++] = ' ';
                }

                continue;
            }

            if (temp[0] == '/' && temp[1] == '/') {
                break;
            }

            if (*temp == '"' || *temp == '\'') {
                char quote = *temp;

                value_buffer[value_index++] = *temp++;

                while (*temp != '\0' && value_index < MAX_VALUE - 1) {

                    size_t literal_continuation =
                        continuation_length(temp);

                    if (literal_continuation > 0) {
                        temp += literal_continuation;
                        continue;
                    }

                    if (*temp == '\n') {
                        break;
                    }

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

        MacroType type = is_function ? M_FUNCTION : macro_type(value_buffer);

        int result = macro_insert(key_buffer, value_buffer, type, params, param_count);

        free(key_buffer);
        free_args(params, param_count);

        return result;
    }

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


    static size_t skip_literal(const char* text, size_t i) {
        char quote = text[i++];

        while (text[i] != '\0') {
            if (text[i] == '\\' && text[i + 1] != '\0') {
                i += 2;
                continue;
            }

            if (text[i] == quote) {
                return i + 1;
            }

            i++;
        }

        return i;
    }

    static int append_text(char* output, size_t* out_index, size_t output_size,
                           const char* text, size_t length) {
        if (*out_index + length >= output_size) {
            definitive_error = 0;
            fprintf(stderr, "Error: expansion de macro demasiado larga\n");
            preprocessor_error_count++;
            return 0;
        }

        memcpy(output + *out_index, text, length);
        *out_index += length;

        return 1;
    }

    static int collect_arguments(const char* text, size_t open, size_t* starts,
                                 size_t* ends, int* count, size_t* close) {
        int depth = 0;
        int found = 0;
        size_t arg_start = open + 1;
        size_t i = open;

        while (text[i] != '\0') {
            char c = text[i];

            if (c == '/' && (text[i + 1] == '*' || text[i + 1] == '/')) {
                i = skip_comment(text, i);
                continue;
            }

            if (c == '"' || c == '\'') {
                i = skip_literal(text, i);
                continue;
            }

            if (c == '(') {
                depth++;
            } else if (c == ')') {
                depth--;

                if (depth == 0) {
                    if (found >= MAX_ARGS) {
                        return -1;
                    }

                    starts[found] = arg_start;
                    ends[found] = i;
                    found++;

                    *count = found;
                    *close = i;

                    return 1;
                }
            } else if (c == ',' && depth == 1) {
                if (found >= MAX_ARGS) {
                    return -1;
                }

                starts[found] = arg_start;
                ends[found] = i;
                found++;

                arg_start = i + 1;
            }

            i++;
        }

        return 0;
    }

    static void trim_range(const char* text, size_t* start, size_t* end) {
        while (*start < *end && isspace((unsigned char)text[*start])) {
            (*start)++;
        }

        while (*end > *start && isspace((unsigned char)text[*end - 1])) {
            (*end)--;
        }
    }

    static int param_index(const Macro* macro, const char* name, size_t length) {
        for (int i = 0; i < macro->arg_count; i++) {
            if (strlen(macro->args[i]) == length &&
                strncmp(macro->args[i], name, length) == 0) {
                return i;
            }
        }

        return -1;
    }

    static int substitute_body(const Macro* macro, char** expanded_args,
                               char* output, size_t output_size) {
        const char* body = macro->value;
        size_t out_index = 0;
        size_t b = 0;

        while (body[b] != '\0') {
            int ok;

            if (body[b] == '"' || body[b] == '\'') {
                size_t end = skip_literal(body, b);
                ok = append_text(output, &out_index, output_size, body + b, end - b);
                b = end;
            } else if (is_ident_start(body[b])) {
                size_t start = b;

                while (is_ident_char(body[b])) {
                    b++;
                }

                int index = param_index(macro, body + start, b - start);

                if (index >= 0) {
                    ok = append_text(output, &out_index, output_size,
                                     expanded_args[index], strlen(expanded_args[index]));
                } else {
                    ok = append_text(output, &out_index, output_size, body + start, b - start);
                }
            } else if (isdigit((unsigned char)body[b])) {
                size_t start = b;

                while (is_ident_char(body[b]) || body[b] == '.') {
                    b++;
                }

                ok = append_text(output, &out_index, output_size, body + start, b - start);
            } else {
                ok = append_text(output, &out_index, output_size, body + b, 1);
                b++;
            }

            if (!ok) {
                return 0;
            }
        }

        output[out_index] = '\0';

        return 1;
    }

    static int expand_function_macro(const Macro* macro, const char* value,
                                     size_t* position, char* output,
                                     size_t* out_index, size_t output_size,
                                     int depth, const Macro** pending) {
        size_t open = *position;

        while (value[open] == ' ' || value[open] == '\t' ||
               value[open] == '\r' || value[open] == '\n') {
            open++;
        }

        if (value[open] != '(') {
            return 0;
        }

        size_t starts[MAX_ARGS];
        size_t ends[MAX_ARGS];
        size_t close = 0;
        int argc = 0;

        int status = collect_arguments(value, open, starts, ends, &argc, &close);

        if (status == 0) {
            fprintf(stderr, "Error: llamada a macro %s sin cerrar\n", macro->key);
            definitive_error = 0;
            preprocessor_error_count++;
            return -1;
        }

        if (status < 0) {
            fprintf(stderr, "Error: demasiados argumentos para la macro %s\n", macro->key);
            preprocessor_error_count++;
            definitive_error = 0;
            return -1;
        }

        for (int i = 0; i < argc; i++) {
            trim_range(value, &starts[i], &ends[i]);
        }

        if (macro->arg_count == 0 && argc == 1 && starts[0] == ends[0]) {
            argc = 0;
        }

        if (argc != macro->arg_count) {
            fprintf(stderr, "Error: la macro %s espera %d argumentos y recibio %d\n",
                    macro->key, macro->arg_count, argc);
            preprocessor_error_count++;
            definitive_error = 0;
            return -1;
        }

        char* expanded_args[MAX_ARGS] = {0};
        char* substituted = malloc(MAX_VALUE);
        char* rescanned = malloc(MAX_VALUE);
        int ok = (substituted != NULL && rescanned != NULL);

        for (int i = 0; ok && i < argc; i++) {
            size_t length = ends[i] - starts[i];
            char* raw = malloc(length + 1);
            expanded_args[i] = malloc(MAX_VALUE);

            if (raw == NULL || expanded_args[i] == NULL) {
                ok = 0;
            } else {
                memcpy(raw, value + starts[i], length);
                raw[length] = '\0';
                ok = expand_macro_value(raw, expanded_args[i], MAX_VALUE,
                                        depth + 1, NULL);
            }

            free(raw);
        }

        if (ok) {
            ok = substitute_body(macro, expanded_args, substituted, MAX_VALUE);
        }

        const Macro* rescan_pending = NULL;

        if (ok) {

            int at_tail = (pending != NULL &&
                           only_blanks_left(value + close + 1));

            ok = expand_macro_value(substituted, rescanned, MAX_VALUE,
                                    depth + 1,
                                    at_tail ? &rescan_pending : NULL);
        }

        if (ok) {
            ok = append_text(output, out_index, output_size, rescanned, strlen(rescanned));
        }

        if (ok && rescan_pending != NULL) {
            *pending = rescan_pending;
        }

        free_args(expanded_args, argc);
        free(substituted);
        free(rescanned);

        *position = close + 1;

        return ok ? 1 : -1;
    }

    static int expand_macro_value(const char* value, char* output,
                              size_t output_size, int depth,
                              const Macro** pending) {
        if (depth >= MAX_DEPTH) {
            fprintf(stderr, "Error: profundidad maxima de macros alcanzada para %s\n", value);
            definitive_error = 0;
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

                size_t id_start = i;

                while (value[i] == '_' ||
                    isalnum((unsigned char)value[i])) {
                    i++;
                }

                size_t id_length = i - id_start;

                char *identifier = malloc(id_length + 1);

                if (identifier == NULL) {
                    fprintf(stderr,
                            "Error: no se pudo reservar memoria para el identificador\n");
                    preprocessor_error_count++;
                    definitive_error = 0;
                    return 0;
                }

                memcpy(identifier, value + id_start, id_length);
                identifier[id_length] = '\0';

                const Macro *macro = macro_find(identifier);

                if (macro != NULL && macro->type == M_FUNCTION) {

                    size_t next = i;
                    int status = expand_function_macro(macro, value, &next, output,
                                                       &out_index, output_size, depth,
                                                       pending);

                    if (status < 0) {
                        free(identifier);
                        return 0;
                    }

                    if (status == 1) {
                        i = next;

                        if (pending != NULL && *pending != NULL) {
                            free(identifier);
                            break;
                        }

                    } else if (pending != NULL && only_blanks_left(value + i)) {

                        *pending = macro;
                        free(identifier);
                        break;
                    } else if (!append_text(output, &out_index, output_size,
                                            identifier, id_length)) {
                        free(identifier);
                        return 0;
                    }

                } else if (macro != NULL) {

                    const char *replacement = macro->value;

                    char expanded[MAX_VALUE];
                    const Macro *nested_pending = NULL;

                    int at_tail = (pending != NULL && only_blanks_left(value + i));

                    if (!expand_macro_value(
                            replacement,
                            expanded,
                            sizeof(expanded),
                            depth + 1,
                            at_tail ? &nested_pending : NULL)) {

                        free(identifier);
                        return 0;
                    }

                    size_t length = strlen(expanded);

                    if (out_index + length >= output_size) {
                        fprintf(stderr, "Error: expansión de macro demasiado larga\n");
                    definitive_error = 0;

                        preprocessor_error_count++;
                        free(identifier);
                        return 0;
                    }

                    memcpy(output + out_index, expanded, length);
                    out_index += length;

                    if (nested_pending != NULL) {
                        *pending = nested_pending;
                        free(identifier);
                        break;
                    }

                } else {

                    if (out_index + id_length >= output_size) {
                        fprintf(stderr, "Error: expansión de macro demasiado larga\n");

                        preprocessor_error_count++;
                        definitive_error = 0;
                        free(identifier);
                        return 0;
                    }

                    memcpy(output + out_index, identifier, id_length);
                    out_index += id_length;
                }

                free(identifier);

            } else if (isdigit((unsigned char)value[i])) {

                size_t number_start = i;

                while (is_ident_char(value[i]) || value[i] == '.') {
                    i++;
                }

                if (!append_text(output, &out_index, output_size,
                                 value + number_start, i - number_start)) {
                    return 0;
                }

            } else {

                if (out_index + 1 >= output_size) {
                    fprintf(stderr, "Error: expansión de macro demasiado larga\n");
                            definitive_error = 0;

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

%option prefix="pp_"

%x COMMENT INCLUDE

CONT    \\[ \t\r]*\n
REST    ({CONT}|[^\n])*

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
^[ \t]*#[ \t]*define[ \t]+[A-Za-z0-9_]+{REST}"/*"([^*]|\*+[^*/])*\*+"/"{REST} {
    macro_handle_define(yytext);
    keep_line_count(yytext);
}

^[ \t]*#[ \t]*define[ \t]+[A-Za-z0-9_]+{REST} {
    macro_handle_define(yytext);
    keep_line_count(yytext);
}

^[ \t]*#[ \t]*include[ \t]*\n {
    fprintf(stderr, "Error: directiva include incompleta\n");
    definitive_error = 0;
    preprocessor_error_count++;
}

^[ \t]*#[ \t]*include"/*" {
    comment_return_state = INCLUDE;
    BEGIN(COMMENT);
}

^[ \t]*#[ \t]*include[ \t]+ {
    BEGIN(INCLUDE);
}


^[ \t]*#[ \t]*include {
    BEGIN(INCLUDE);
}

<INCLUDE>[ \t]+ {;}

<INCLUDE>\n {
    fprintf(stderr, "Error: directiva include incompleta\n");
    definitive_error = 0;
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
        char resolved_path[PATH_MAX + MAX_VALUE + 2];
        FILE* included_file = NULL;

        if (filename[0] != '/') {
            const char* current_dir = include_dirs[include_depth];
            snprintf(resolved_path, sizeof(resolved_path), "%s/%s", current_dir, filename);
            included_file = fopen(resolved_path, "r");
        }

        if (included_file == NULL) {
            included_file = fopen(filename, "r");
            if (included_file != NULL) {
                strncpy(resolved_path, filename, sizeof(resolved_path) - 1);
                resolved_path[sizeof(resolved_path) - 1] = '\0';
            }
        }

        if (included_file == NULL) {
            fprintf(stderr, "Error: no se pudo abrir %s\n", filename);
            definitive_error = 0;
            preprocessor_error_count++;
        } else if (include_depth >= MAX_DEPTH) {
            fprintf(stderr, "Error: demasiados includes anidados\n");
            preprocessor_error_count++;
            definitive_error = 0;
            fclose(included_file);
        } else {
            get_dirname(resolved_path, include_dirs[include_depth + 1], sizeof(include_dirs[include_depth + 1]));
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

[0-9][A-Za-z0-9_.]* {
    fprintf(yyout, "%s", yytext);
}

[A-Za-z_][A-Za-z0-9_]* {
    const Macro* macro = macro_find(yytext);

    if (macro != NULL && macro->type == M_FUNCTION) {
        handle_function_call(macro, yytext);
    } else if (macro != NULL) {
        char expanded[MAX_VALUE];
        const Macro* pending = NULL;

        if (expand_macro_value(macro->value, expanded, sizeof(expanded), 0,
                               &pending)) {
            fprintf(yyout, "%s", expanded);
 
            if (pending != NULL) {
                handle_function_call(pending, pending->key);
            }
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

    fputc(' ', yyout);

    BEGIN(COMMENT);
}

<COMMENT>"*/" {
    BEGIN(comment_return_state);
}
<COMMENT>\n {
    if (comment_return_state == INITIAL) {
        fputc('\n', yyout);
    }
}
<COMMENT>. {;}

<COMMENT><<EOF>> {
    fprintf(stderr, "Error léxico: comentario sin cerrar\n");
    definitive_error = 0;
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
    definitive_error = 0;
    preprocessor_error_count++;

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

int yywrap(){
    return 1;
}

static void handle_function_call(const Macro* macro, const char* name) {
    char call[MAX_VALUE];
    char blanks[MAX_VALUE];
    size_t starts[MAX_ARGS];
    size_t ends[MAX_ARGS];
    size_t call_length = strlen(name);
    size_t blank_length = 0;
    size_t open = call_length;
    size_t close = 0;
    int argc = 0;
    int newlines = 0;
    int terminated = 0;
    int c;

    if (call_length >= sizeof(call) - 1) {
        fprintf(yyout, "%s", name);
        return;
    }

    memcpy(call, name, call_length);
    call[call_length] = '\0';

    while (1) {
        c = input();
        if (c == EOF || c == 0) {
            break;
        }

        if (isspace(c)) {
            if (c == '\n') {
                newlines++;
            }
            if (blank_length < sizeof(blanks) - 1) {
                blanks[blank_length++] = (char)c;
            }
            continue;
        }

        if (c == '/') {
            int c2 = input();
            if (c2 == '*') {
                if (blank_length < sizeof(blanks) - 1) {
                    blanks[blank_length++] = ' ';
                }
                int prev = 0;
                while ((c = input()) != EOF && c != 0) {
                    if (c == '\n') newlines++;
                    if (prev == '*' && c == '/') break;
                    prev = c;
                }
                continue;
            } else if (c2 == '/') {
                if (blank_length < sizeof(blanks) - 1) {
                    blanks[blank_length++] = ' ';
                }
                while ((c = input()) != EOF && c != 0 && c != '\n') {
                }
                if (c == '\n') newlines++;
                continue;
            } else {
                if (c2 != EOF && c2 != 0) {
                    unput(c2);
                }
            }
        }

        break;
    }

    blanks[blank_length] = '\0';

    if (c != '(') {
        fprintf(yyout, "%s%s", call, blanks);

        if (c != EOF && c != 0) {
            unput(c);
        }

        return;
    }

    call[call_length++] = '(';
    call[call_length] = '\0';

    while (call_length < sizeof(call) - 1) {
        c = input();

        if (c == EOF || c == 0) {
            break;
        }

        if (c == '/') {
            int c2 = input();
            if (c2 == '*') {
                if (call_length < sizeof(call) - 1) {
                    call[call_length++] = ' ';
                }
                int prev = 0;
                while ((c = input()) != EOF && c != 0) {
                    if (c == '\n') newlines++;
                    if (prev == '*' && c == '/') break;
                    prev = c;
                }
                continue;
            } else if (c2 == '/') {
                if (call_length < sizeof(call) - 1) {
                    call[call_length++] = ' ';
                }
                while ((c = input()) != EOF && c != 0 && c != '\n') {
                }
                if (c == '\n') newlines++;
                continue;
            } else {
                if (c2 != EOF && c2 != 0) {
                    unput(c2);
                }
            }
        }

        if (c == '\n') {
            newlines++;
            c = ' ';
        }

        call[call_length++] = (char)c;
        call[call_length] = '\0';

        if (c == ')' &&
            collect_arguments(call, open, starts, ends, &argc, &close) != 0) {

            terminated = 1;
            break;
        }
    }

    call[call_length] = '\0';

    if (!terminated) {
        fprintf(stderr, "Error: llamada a macro %s sin cerrar\n", macro->key);
        definitive_error = 0;
        preprocessor_error_count++;
        fprintf(yyout, "%s", call);
    } else {
        char expanded[MAX_VALUE];
        const Macro* pending = NULL;

        if (expand_macro_value(call, expanded, sizeof(expanded), 0, &pending)) {
            fprintf(yyout, "%s", expanded);

            if (pending != NULL) {
                handle_function_call(pending, pending->key);
            }
        } else {
            fprintf(yyout, "%s", call);
        }
    }

    for (int i = 0; i < newlines; i++) {
        fputc('\n', yyout);
    }
}

int preprocess_file(
    const char *input_filename,
    char *temp_filename,
    size_t temp_filename_size,
    int *lexical_errors,
    int *preprocessor_errors
) {
    if (input_filename == NULL ||
        temp_filename == NULL ||
        temp_filename_size == 0) {

        return 0;
    }

    FILE *input_file = fopen(input_filename, "r");

    if (input_file == NULL) {
        fprintf(
            stderr,
            "Error: no se pudo abrir %s\n",
            input_filename
        );
        return 0;
    }

    char temp_template[] = "/tmp/preprocessor_XXXXXX";

    int temp_fd = mkstemp(temp_template);

    if (temp_fd == -1) {
        fprintf(
            stderr,
            "Error: no se pudo crear el archivo temporal\n"
        );

        fclose(input_file);

        return 0;
    }

    FILE *temp_file = fdopen(temp_fd, "w");

    if (temp_file == NULL) {
        fprintf(
            stderr,
            "Error: no se pudo abrir el archivo temporal\n"
        );

        close(temp_fd);
        fclose(input_file);

        return 0;
    }

    free_macro_table();
    definitive_error = 1;
    lexical_error_count = 0;
    preprocessor_error_count = 0;
    include_depth = 0;
    get_dirname(input_filename, include_dirs[0], sizeof(include_dirs[0]));

    pp_in = input_file;
    pp_out = temp_file;

    pp_lex();

    fclose(input_file);
    fclose(temp_file);

    if (strlen(temp_template) + 1 > temp_filename_size) {
        fprintf(
            stderr,
            "Error: nombre de archivo temporal demasiado largo\n"
        );

        return 0;
    }

    strcpy(temp_filename, temp_template);

    if (lexical_errors != NULL) {
        *lexical_errors = lexical_error_count;
    }

    if (preprocessor_errors != NULL) {
        *preprocessor_errors = preprocessor_error_count;
    }

    return definitive_error;
}