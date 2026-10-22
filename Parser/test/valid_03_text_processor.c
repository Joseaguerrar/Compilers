/*
 * valid_03_text_processor.c
 * Programa de prueba sintácticamente válido para C99/C11.
 * Procesador de texto, manipulación de hileras, parser CSV,
 * cifradores (César, Vigenère), codificador Base64 y estadísticas de texto.
 * Líneas: > 500
 */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>


#define STRING_BUILDER_INIT_CAPACITY 32
#define MAX_CSV_FIELDS 64
#define MAX_WORD_LENGTH 64
#define BASE64_PAD '='

/* ========================================================================= */
/* 1. Constructor Dinámico de Cadenas (StringBuilder)                       */
/* ========================================================================= */

typedef struct {
    char *buffer;
    size_t length;
    size_t capacity;
} StringBuilder;

StringBuilder *sb_create(void) {
    StringBuilder *sb = (StringBuilder *)malloc(sizeof(StringBuilder));
    if (sb == NULL) {
        return NULL;
    }
    sb->capacity = STRING_BUILDER_INIT_CAPACITY;
    sb->length = 0;
    sb->buffer = (char *)malloc(sb->capacity);
    if (sb->buffer == NULL) {
        free(sb);
        return NULL;
    }
    sb->buffer[0] = '\0';
    return sb;
}

static bool sb_ensure_capacity(StringBuilder *sb, size_t needed) {
    if (sb->length + needed + 1 > sb->capacity) {
        size_t new_cap = sb->capacity * 2;
        while (sb->length + needed + 1 > new_cap) {
            new_cap *= 2;
        }
        char *new_buf = (char *)realloc(sb->buffer, new_cap);
        if (new_buf == NULL) {
            return false;
        }
        sb->buffer = new_buf;
        sb->capacity = new_cap;
    }
    return true;
}

bool sb_append(StringBuilder *sb, const char *str) {
    if (sb == NULL || str == NULL) {
        return false;
    }
    size_t str_len = strlen(str);
    if (!sb_ensure_capacity(sb, str_len)) {
        return false;
    }
    memcpy(sb->buffer + sb->length, str, str_len);
    sb->length += str_len;
    sb->buffer[sb->length] = '\0';
    return true;
}

bool sb_append_char(StringBuilder *sb, char c) {
    if (sb == NULL) {
        return false;
    }
    if (!sb_ensure_capacity(sb, 1)) {
        return false;
    }
    sb->buffer[sb->length] = c;
    sb->length++;
    sb->buffer[sb->length] = '\0';
    return true;
}

bool sb_append_int(StringBuilder *sb, int num) {
    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%d", num);
    return sb_append(sb, tmp);
}

void sb_clear(StringBuilder *sb) {
    if (sb != NULL) {
        sb->length = 0;
        if (sb->buffer != NULL) {
            sb->buffer[0] = '\0';
        }
    }
}

char *sb_to_string(const StringBuilder *sb) {
    if (sb == NULL || sb->buffer == NULL) {
        return NULL;
    }
    char *copy = (char *)malloc(sb->length + 1);
    if (copy != NULL) {
        memcpy(copy, sb->buffer, sb->length + 1);
    }
    return copy;
}

void sb_destroy(StringBuilder *sb) {
    if (sb != NULL) {
        free(sb->buffer);
        free(sb);
    }
}

/* ========================================================================= */
/* 2. Tokenizador de Texto Básico                                           */
/* ========================================================================= */

typedef struct {
    char **tokens;
    size_t count;
    size_t capacity;
} TokenList;

TokenList *token_list_create(void) {
    TokenList *tl = (TokenList *)malloc(sizeof(TokenList));
    if (tl == NULL) {
        return NULL;
    }
    tl->capacity = 16;
    tl->count = 0;
    tl->tokens = (char **)malloc(tl->capacity * sizeof(char *));
    if (tl->tokens == NULL) {
        free(tl);
        return NULL;
    }
    return tl;
}

bool token_list_add(TokenList *tl, const char *token) {
    if (tl == NULL || token == NULL) {
        return false;
    }
    if (tl->count >= tl->capacity) {
        size_t new_cap = tl->capacity * 2;
        char **new_toks = (char **)realloc(tl->tokens, new_cap * sizeof(char *));
        if (new_toks == NULL) {
            return false;
        }
        tl->tokens = new_toks;
        tl->capacity = new_cap;
    }
    tl->tokens[tl->count] = strdup(token);
    if (tl->tokens[tl->count] == NULL) {
        return false;
    }
    tl->count++;
    return true;
}

void token_list_destroy(TokenList *tl) {
    if (tl == NULL) {
        return;
    }
    for (size_t i = 0; i < tl->count; i++) {
        free(tl->tokens[i]);
    }
    free(tl->tokens);
    free(tl);
}

TokenList *text_tokenize(const char *input, const char *delims) {
    if (input == NULL || delims == NULL) {
        return NULL;
    }
    TokenList *tl = token_list_create();
    if (tl == NULL) {
        return NULL;
    }
    char *copy = strdup(input);
    if (copy == NULL) {
        token_list_destroy(tl);
        return NULL;
    }

    char *saveptr = NULL;
    char *tok = strtok_r(copy, delims, &saveptr);
    while (tok != NULL) {
        token_list_add(tl, tok);
        tok = strtok_r(NULL, delims, &saveptr);
    }

    free(copy);
    return tl;
}

/* ========================================================================= */
/* 3. Analizador de Archivos CSV Simple                                     */
/* ========================================================================= */

typedef struct {
    char *fields[MAX_CSV_FIELDS];
    size_t field_count;
} CSVRow;

bool csv_parse_line(const char *line, CSVRow *row) {
    if (line == NULL || row == NULL) {
        return false;
    }
    row->field_count = 0;

    StringBuilder *sb = sb_create();
    if (sb == NULL) {
        return false;
    }

    bool in_quotes = false;
    size_t len = strlen(line);

    for (size_t i = 0; i < len; i++) {
        char c = line[i];

        if (c == '\"') {
            if (in_quotes && i + 1 < len && line[i + 1] == '\"') {
                sb_append_char(sb, '\"');
                i++;
            } else {
                in_quotes = !in_quotes;
            }
        } else if (c == ',' && !in_quotes) {
            if (row->field_count < MAX_CSV_FIELDS) {
                row->fields[row->field_count] = sb_to_string(sb);
                row->field_count++;
            }
            sb_clear(sb);
        } else if (c == '\r' || c == '\n') {
            break;
        } else {
            sb_append_char(sb, c);
        }
    }

    if (row->field_count < MAX_CSV_FIELDS) {
        row->fields[row->field_count] = sb_to_string(sb);
        row->field_count++;
    }

    sb_destroy(sb);
    return true;
}

void csv_row_free(CSVRow *row) {
    if (row == NULL) {
        return;
    }
    for (size_t i = 0; i < row->field_count; i++) {
        free(row->fields[i]);
        row->fields[i] = NULL;
    }
    row->field_count = 0;
}

/* ========================================================================= */
/* 4. Cifrados Clásicos: César y Vigenère                                   */
/* ========================================================================= */

char *cipher_caesar_encrypt(const char *plaintext, int shift) {
    if (plaintext == NULL) {
        return NULL;
    }
    size_t len = strlen(plaintext);
    char *result = (char *)malloc(len + 1);
    if (result == NULL) {
        return NULL;
    }
    shift = ((shift % 26) + 26) % 26;

    for (size_t i = 0; i < len; i++) {
        char c = plaintext[i];
        if (isupper((unsigned char)c)) {
            result[i] = (char)(((c - 'A' + shift) % 26) + 'A');
        } else if (islower((unsigned char)c)) {
            result[i] = (char)(((c - 'a' + shift) % 26) + 'a');
        } else {
            result[i] = c;
        }
    }
    result[len] = '\0';
    return result;
}

char *cipher_caesar_decrypt(const char *ciphertext, int shift) {
    return cipher_caesar_encrypt(ciphertext, 26 - (shift % 26));
}

char *cipher_vigenere_encrypt(const char *plaintext, const char *key) {
    if (plaintext == NULL || key == NULL || strlen(key) == 0) {
        return NULL;
    }
    size_t text_len = strlen(plaintext);
    size_t key_len = strlen(key);
    char *result = (char *)malloc(text_len + 1);
    if (result == NULL) {
        return NULL;
    }

    size_t key_idx = 0;
    for (size_t i = 0; i < text_len; i++) {
        char c = plaintext[i];
        if (isalpha((unsigned char)c)) {
            char k = (char)tolower((unsigned char)key[key_idx % key_len]);
            int shift = k - 'a';
            if (isupper((unsigned char)c)) {
                result[i] = (char)(((c - 'A' + shift) % 26) + 'A');
            } else {
                result[i] = (char)(((c - 'a' + shift) % 26) + 'a');
            }
            key_idx++;
        } else {
            result[i] = c;
        }
    }
    result[text_len] = '\0';
    return result;
}

char *cipher_vigenere_decrypt(const char *ciphertext, const char *key) {
    if (ciphertext == NULL || key == NULL || strlen(key) == 0) {
        return NULL;
    }
    size_t text_len = strlen(ciphertext);
    size_t key_len = strlen(key);
    char *result = (char *)malloc(text_len + 1);
    if (result == NULL) {
        return NULL;
    }

    size_t key_idx = 0;
    for (size_t i = 0; i < text_len; i++) {
        char c = ciphertext[i];
        if (isalpha((unsigned char)c)) {
            char k = (char)tolower((unsigned char)key[key_idx % key_len]);
            int shift = k - 'a';
            if (isupper((unsigned char)c)) {
                result[i] = (char)(((c - 'A' - shift + 26) % 26) + 'A');
            } else {
                result[i] = (char)(((c - 'a' - shift + 26) % 26) + 'a');
            }
            key_idx++;
        } else {
            result[i] = c;
        }
    }
    result[text_len] = '\0';
    return result;
}

/* ========================================================================= */
/* 5. Codificador y Decodificador Base64                                    */
/* ========================================================================= */

static const char b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char *base64_encode(const unsigned char *data, size_t input_length) {
    if (data == NULL) {
        return NULL;
    }
    size_t output_length = 4 * ((input_length + 2) / 3);
    char *encoded_data = (char *)malloc(output_length + 1);
    if (encoded_data == NULL) {
        return NULL;
    }

    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        unsigned int octet_a = (i < input_length) ? (unsigned char)data[i++] : 0;
        unsigned int octet_b = (i < input_length) ? (unsigned char)data[i++] : 0;
        unsigned int octet_c = (i < input_length) ? (unsigned char)data[i++] : 0;

        unsigned int triple = (octet_a << 16) + (octet_b << 8) + octet_c;

        encoded_data[j++] = b64_table[(triple >> 18) & 0x3F];
        encoded_data[j++] = b64_table[(triple >> 12) & 0x3F];
        encoded_data[j++] = (i > input_length + 1) ? BASE64_PAD : b64_table[(triple >> 6) & 0x3F];
        encoded_data[j++] = (i > input_length) ? BASE64_PAD : b64_table[triple & 0x3F];
    }
    encoded_data[output_length] = '\0';
    return encoded_data;
}

unsigned char *base64_decode(const char *data, size_t *out_len) {
    if (data == NULL) {
        return NULL;
    }
    size_t input_length = strlen(data);
    if (input_length % 4 != 0) {
        return NULL;
    }

    size_t output_length = input_length / 4 * 3;
    if (input_length > 0 && data[input_length - 1] == BASE64_PAD) output_length--;
    if (input_length > 1 && data[input_length - 2] == BASE64_PAD) output_length--;

    unsigned char *decoded_data = (unsigned char *)malloc(output_length);
    if (decoded_data == NULL) {
        return NULL;
    }

    int decoding_table[256];
    for (int idx = 0; idx < 256; idx++) decoding_table[idx] = -1;
    for (int idx = 0; idx < 64; idx++) decoding_table[(unsigned char)b64_table[idx]] = idx;

    size_t i, j;
    for (i = 0, j = 0; i < input_length;) {
        int sextet_a = data[i] == BASE64_PAD ? 0 : decoding_table[(unsigned char)data[i]]; i++;
        int sextet_b = data[i] == BASE64_PAD ? 0 : decoding_table[(unsigned char)data[i]]; i++;
        int sextet_c = data[i] == BASE64_PAD ? 0 : decoding_table[(unsigned char)data[i]]; i++;
        int sextet_d = data[i] == BASE64_PAD ? 0 : decoding_table[(unsigned char)data[i]]; i++;

        unsigned int triple = (unsigned int)((sextet_a << 18) + (sextet_b << 12) + (sextet_c << 6) + sextet_d);

        if (j < output_length) decoded_data[j++] = (unsigned char)((triple >> 16) & 0xFF);
        if (j < output_length) decoded_data[j++] = (unsigned char)((triple >> 8) & 0xFF);
        if (j < output_length) decoded_data[j++] = (unsigned char)(triple & 0xFF);
    }

    if (out_len != NULL) {
        *out_len = output_length;
    }
    return decoded_data;
}

/* ========================================================================= */
/* 6. Estadísticas y Conteo de Palabras                                     */
/* ========================================================================= */

typedef struct {
    size_t line_count;
    size_t word_count;
    size_t char_count;
    size_t letter_count;
    size_t digit_count;
} TextStats;

TextStats text_analyze(const char *text) {
    TextStats stats;
    memset(&stats, 0, sizeof(stats));
    if (text == NULL) {
        return stats;
    }

    bool in_word = false;
    size_t len = strlen(text);

    for (size_t i = 0; i < len; i++) {
        char c = text[i];
        stats.char_count++;

        if (c == '\n') {
            stats.line_count++;
        }
        if (isalpha((unsigned char)c)) {
            stats.letter_count++;
        }
        if (isdigit((unsigned char)c)) {
            stats.digit_count++;
        }

        if (isspace((unsigned char)c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            stats.word_count++;
        }
    }

    if (len > 0 && text[len - 1] != '\n') {
        stats.line_count++;
    }
    return stats;
}

/* ========================================================================= */
/* 7. Función Principal de Pruebas (Driver)                                 */
/* ========================================================================= */

int main(void) {
    printf("=== Probando StringBuilder ===\n");
    StringBuilder *sb = sb_create();
    sb_append(sb, "Compiladores ");
    sb_append(sb, "UCR ");
    sb_append_int(sb, 2026);
    char *built = sb_to_string(sb);
    printf("Construido: %s\n", built);
    free(built);
    sb_destroy(sb);

    printf("\n=== Probando Tokenizador ===\n");
    const char *sample_sentence = "flex bison gcc make linux bash";
    TokenList *tokens = text_tokenize(sample_sentence, " ");
    if (tokens != NULL) {
        printf("Tokens encontrados (%zu):\n", tokens->count);
        for (size_t i = 0; i < tokens->count; i++) {
            printf("  [%zu]: %s\n", i, tokens->tokens[i]);
        }
        token_list_destroy(tokens);
    }

    printf("\n=== Probando Parser CSV ===\n");
    const char *csv_sample = "101,\"Torres, Rojas\",Ingenieria,100.0";
    CSVRow row;
    if (csv_parse_line(csv_sample, &row)) {
        printf("Campos CSV (%zu):\n", row.field_count);
        for (size_t i = 0; i < row.field_count; i++) {
            printf("  Col %zu: %s\n", i, row.fields[i]);
        }
        csv_row_free(&row);
    }

    printf("\n=== Probando Cifrado Cesar y Vigenere ===\n");
    const char *msg = "Ataque al amanecer en el campus!";
    char *c_enc = cipher_caesar_encrypt(msg, 3);
    char *c_dec = cipher_caesar_decrypt(c_enc, 3);
    printf("Cesar Cifrado: %s\n", c_enc);
    printf("Cesar Descifrado: %s\n", c_dec);
    free(c_enc);
    free(c_dec);

    char *v_enc = cipher_vigenere_encrypt(msg, "CLAVE");
    char *v_dec = cipher_vigenere_decrypt(v_enc, "CLAVE");
    printf("Vigenere Cifrado: %s\n", v_enc);
    printf("Vigenere Descifrado: %s\n", v_dec);
    free(v_enc);
    free(v_dec);

    printf("\n=== Probando Base64 ===\n");
    const char *b64_input = "Proyecto 2 de Compiladores 2026";
    char *b64_enc = base64_encode((const unsigned char *)b64_input, strlen(b64_input));
    printf("Base64 Enc: %s\n", b64_enc);
    size_t dec_len = 0;
    unsigned char *b64_dec = base64_decode(b64_enc, &dec_len);
    if (b64_dec != NULL) {
        printf("Base64 Dec: %.*s\n", (int)dec_len, b64_dec);
        free(b64_dec);
    }
    free(b64_enc);

    printf("\n=== Probando Analizador de Texto ===\n");
    const char *parrafo = "Primera linea con cinco palabras.\nSegunda linea con numeros 123 y letras.\nTercera linea final.\n";
    TextStats stats = text_analyze(parrafo);
    printf("Lineas: %zu, Palabras: %zu, Caracteres: %zu, Letras: %zu, Digitos: %zu\n",
           stats.line_count, stats.word_count, stats.char_count, stats.letter_count, stats.digit_count);

    printf("\n=== Todas las pruebas de texto completadas con exito ===\n");
    return 0;
}
