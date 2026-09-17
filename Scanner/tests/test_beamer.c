#include <stdio.h>
#include <stddef.h>

#include "beamer_generator.h"

int main(void) {

    Token tokens[] = {

        {
            .code = TOK_INT,
            .category = CAT_KEYWORD,
            .lexeme = "int",
            .line = 1,
            .column = 1
        },

        {
            .code = TOK_IDENTIFIER,
            .category = CAT_IDENTIFIER,
            .lexeme = "main",
            .line = 1,
            .column = 5
        },

        {
            .code = TOK_LPAREN,
            .category = CAT_PUNCTUATOR,
            .lexeme = "(",
            .line = 1,
            .column = 9
        },

        {
            .code = TOK_RPAREN,
            .category = CAT_PUNCTUATOR,
            .lexeme = ")",
            .line = 1,
            .column = 10
        },

        {
            .code = TOK_LBRACE,
            .category = CAT_PUNCTUATOR,
            .lexeme = "{",
            .line = 1,
            .column = 12
        },

        {
            .code = TOK_INT,
            .category = CAT_KEYWORD,
            .lexeme = "int",
            .line = 2,
            .column = 5
        },

        {
            .code = TOK_IDENTIFIER,
            .category = CAT_IDENTIFIER,
            .lexeme = "value",
            .line = 2,
            .column = 9
        },

        {
            .code = TOK_ASSIGN,
            .category = CAT_OPERATOR,
            .lexeme = "=",
            .line = 2,
            .column = 15
        },

        {
            .code = TOK_INT_CONST,
            .category = CAT_CONSTANT,
            .lexeme = "10",
            .line = 2,
            .column = 17
        },

        {
            .code = TOK_SEMICOLON,
            .category = CAT_PUNCTUATOR,
            .lexeme = ";",
            .line = 2,
            .column = 19
        },

        {
            .code = TOK_STRING_LITERAL,
            .category = CAT_STRING_LITERAL,
            .lexeme = "\"hola\"",
            .line = 3,
            .column = 5
        },

        {
            .code = TOK_CHAR_CONST,
            .category = CAT_CONSTANT,
            .lexeme = "'a'",
            .line = 4,
            .column = 5
        },

        {
            .code = TOK_RETURN,
            .category = CAT_KEYWORD,
            .lexeme = "return",
            .line = 5,
            .column = 5
        },

        {
            .code = TOK_IDENTIFIER,
            .category = CAT_IDENTIFIER,
            .lexeme = "value",
            .line = 5,
            .column = 12
        },

        {
            .code = TOK_SEMICOLON,
            .category = CAT_PUNCTUATOR,
            .lexeme = ";",
            .line = 5,
            .column = 17
        },

        {
            .code = TOK_ERROR,
            .category = CAT_ERROR,
            .lexeme = "@",
            .line = 6,
            .column = 5
        },

        {
            .code = TOK_RBRACE,
            .category = CAT_PUNCTUATOR,
            .lexeme = "}",
            .line = 7,
            .column = 1
        },

        {
            .code = TOK_EOF,
            .category = CAT_EOF,
            .lexeme = "",
            .line = 8,
            .column = 1
        }
    };

    size_t token_count =
        sizeof(tokens) / sizeof(tokens[0]);

    if (!beamer_generate(
            "presentation/presentation.tex",
            "tests/example.c",
            "/tmp/preprocessor_ABC123",
            tokens,
            token_count)) {

        fprintf(
            stderr,
            "Error generando la presentación\n"
        );

        return 1;
    }

    printf(
        "Presentación generada correctamente: "
        "presentation/presentation.tex\n"
    );

    return 0;
}