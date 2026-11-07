%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"

#define TOKEN_LIST(X) \
    X(AUTO) X(BREAK) X(CASE) X(CHAR) X(CONST) X(CONTINUE) \
    X(DEFAULT) X(DO) X(DOUBLE) X(ELSE) X(ENUM) X(EXTERN) \
    X(FLOAT) X(FOR) X(GOTO) X(IF) X(INT) X(LONG) \
    X(REGISTER) X(RETURN) X(SHORT) X(SIGNED) X(SIZEOF) X(STATIC) \
    X(STRUCT) X(SWITCH) X(TYPEDEF) X(UNION) X(UNSIGNED) X(VOID) \
    X(VOLATILE) X(WHILE) X(INLINE) X(RESTRICT) X(ALIGNAS) X(ALIGNOF) \
    X(ATOMIC) X(BOOL) X(COMPLEX) X(GENERIC) X(IMAGINARY) X(NORETURN) \
    X(STATIC_ASSERT) X(THREAD_LOCAL) X(IDENTIFIER) X(STRING_LITERAL) X(INT_CONST) X(FLOAT_CONST) \
    X(CHAR_CONST) X(PLUS) X(MINUS) X(STAR) X(SLASH) X(PERCENT) \
    X(ASSIGN) X(EQ) X(NEQ) X(LT) X(GT) X(LE) \
    X(GE) X(AND) X(OR) X(NOT) X(BIT_AND) X(BIT_OR) \
    X(BIT_XOR) X(BIT_NOT) X(LSHIFT) X(RSHIFT) X(INCREMENT) X(DECREMENT) \
    X(ARROW) X(PLUS_ASSIGN) X(MINUS_ASSIGN) X(STAR_ASSIGN) X(SLASH_ASSIGN) X(PERCENT_ASSIGN) \
    X(BIT_AND_ASSIGN) X(BIT_OR_ASSIGN) X(BIT_XOR_ASSIGN) X(LSHIFT_ASSIGN) X(RSHIFT_ASSIGN) X(LPAREN) \
    X(RPAREN) X(LBRACE) X(RBRACE) X(LBRACKET) X(RBRACKET) X(SEMICOLON) \
    X(COMMA) X(DOT) X(COLON) X(QUESTION) X(ELLIPSIS) X(HASH) \
    X(DOUBLE_HASH) X(ERROR)

static int yylex(void);
static void yyerror(const char *msg);

static int syntax_error_count = 0;
%}

%define api.token.prefix {T_}

%union {
    long long ival; /* TOK_INT_CONST */
    double fval;    /* TOK_FLOAT_CONST */
    char cval;      /* TOK_CHAR_CONST */
    char *sval;     /* TOK_IDENTIFIER, TOK_STRING_LITERAL */
}

/* Keywords */
%token AUTO BREAK CASE CHAR CONST CONTINUE
%token DEFAULT DO DOUBLE ELSE ENUM EXTERN
%token FLOAT FOR GOTO IF INT LONG
%token REGISTER RETURN SHORT SIGNED SIZEOF
%token STATIC STRUCT SWITCH TYPEDEF UNION
%token UNSIGNED VOID VOLATILE WHILE
%token INLINE RESTRICT ALIGNAS ALIGNOF ATOMIC BOOL
%token COMPLEX GENERIC IMAGINARY NORETURN
%token STATIC_ASSERT THREAD_LOCAL

/* Identifiers and constants */
%token <sval> IDENTIFIER STRING_LITERAL
%token <ival> INT_CONST
%token <fval> FLOAT_CONST
%token <cval> CHAR_CONST

/* Operators */
%token PLUS MINUS STAR SLASH PERCENT
%token ASSIGN EQ NEQ LT GT LE GE
%token AND OR NOT BIT_AND BIT_OR BIT_XOR
%token BIT_NOT LSHIFT RSHIFT INCREMENT DECREMENT ARROW
%token PLUS_ASSIGN MINUS_ASSIGN STAR_ASSIGN SLASH_ASSIGN
%token PERCENT_ASSIGN BIT_AND_ASSIGN BIT_OR_ASSIGN
%token BIT_XOR_ASSIGN LSHIFT_ASSIGN RSHIFT_ASSIGN

/* Punctuators */
%token LPAREN RPAREN LBRACE RBRACE
%token LBRACKET RBRACKET SEMICOLON COMMA DOT
%token COLON QUESTION ELLIPSIS HASH DOUBLE_HASH

%token ERROR

%start root

%%

/* Gramática de prueba para expresiones.
   Todavía NO reconoce programas C completos. */

root:
      %empty
    | root expression SEMICOLON
    ;

/* Expresiones primarias */
primary_expression:
      IDENTIFIER       { free($1); }
    | INT_CONST
    | FLOAT_CONST
    | CHAR_CONST
    | STRING_LITERAL   { free($1); }
    | LPAREN expression RPAREN
    ;

/* Expresiones postfijas */
postfix_expression:
      primary_expression
    | postfix_expression LBRACKET expression RBRACKET
    | postfix_expression LPAREN RPAREN
    | postfix_expression LPAREN argument_list RPAREN
    | postfix_expression DOT IDENTIFIER { free($3); }
    | postfix_expression ARROW IDENTIFIER { free($3); }
    | postfix_expression INCREMENT
    | postfix_expression DECREMENT
    ;

/* Argumentos de función */
argument_list:
      assignment_expression
    | argument_list COMMA assignment_expression
    ;

/* Expresiones unarias */
unary_expression:
      postfix_expression
    | INCREMENT unary_expression
    | DECREMENT unary_expression
    | PLUS unary_expression
    | MINUS unary_expression
    | NOT unary_expression
    | BIT_NOT unary_expression
    | STAR unary_expression
    | BIT_AND unary_expression
    | SIZEOF unary_expression
    ;

/* Multiplicación, división y módulo */
multiplicative_expression:
      unary_expression
    | multiplicative_expression STAR unary_expression
    | multiplicative_expression SLASH unary_expression
    | multiplicative_expression PERCENT unary_expression
    ;

/* Suma y resta */
additive_expression:
      multiplicative_expression
    | additive_expression PLUS multiplicative_expression
    | additive_expression MINUS multiplicative_expression
    ;

/* Desplazamientos de bits */
shift_expression:
      additive_expression
    | shift_expression LSHIFT additive_expression
    | shift_expression RSHIFT additive_expression
    ;

/* Comparaciones relacionales */
relational_expression:
      shift_expression
    | relational_expression LT shift_expression
    | relational_expression GT shift_expression
    | relational_expression LE shift_expression
    | relational_expression GE shift_expression
    ;

/* Igualdad y desigualdad */
equality_expression:
      relational_expression
    | equality_expression EQ relational_expression
    | equality_expression NEQ relational_expression
    ;

/* AND a nivel de bits */
and_expression:
      equality_expression
    | and_expression BIT_AND equality_expression
    ;

/* XOR a nivel de bits */
exclusive_or_expression:
      and_expression
    | exclusive_or_expression BIT_XOR and_expression
    ;

/* OR a nivel de bits */
inclusive_or_expression:
      exclusive_or_expression
    | inclusive_or_expression BIT_OR exclusive_or_expression
    ;

/* AND logico */
logical_and_expression:
      inclusive_or_expression
    | logical_and_expression AND inclusive_or_expression
    ;

/* OR logico */
logical_or_expression:
      logical_and_expression
    | logical_or_expression OR logical_and_expression
    ;

/* Operador condicional ternario */
conditional_expression:
      logical_or_expression
    | logical_or_expression QUESTION expression COLON conditional_expression
    ;

/* Operadores de asignacion */
assignment_operator:
      ASSIGN
    | PLUS_ASSIGN
    | MINUS_ASSIGN
    | STAR_ASSIGN
    | SLASH_ASSIGN
    | PERCENT_ASSIGN
    | LSHIFT_ASSIGN
    | RSHIFT_ASSIGN
    | BIT_AND_ASSIGN
    | BIT_XOR_ASSIGN
    | BIT_OR_ASSIGN
    ;

/* Expresiones de asignacion */
assignment_expression:
      conditional_expression
    | unary_expression assignment_operator assignment_expression
    ;

/* Entrada de expresión, por ahora */
expression:
      assignment_expression
    | expression COMMA assignment_expression
    ;

%%

static int yylex(void) {
    Token t = Get_Token();

    if (t.code == TOK_EOF) {
        if (t.lexeme) {
            free(t.lexeme);
        }
        return 0;
    }

    int bison_token = T_ERROR;

    switch (t.code) {
        case TOK_IDENTIFIER:
        case TOK_STRING_LITERAL:
            yylval.sval = t.lexeme;
            return (t.code == TOK_IDENTIFIER) ? T_IDENTIFIER : T_STRING_LITERAL;

        case TOK_INT_CONST:
            yylval.ival = strtoll(t.lexeme, NULL, 0);
            free(t.lexeme);
            return T_INT_CONST;

        case TOK_FLOAT_CONST:
            yylval.fval = strtod(t.lexeme, NULL);
            free(t.lexeme);
            return T_FLOAT_CONST;

        case TOK_CHAR_CONST:
            yylval.cval = (t.lexeme && t.lexeme[1]) ? t.lexeme[1] : '\0';
            free(t.lexeme);
            return T_CHAR_CONST;

        default:
            /* Liberar el lexema para operadores, palabras reservadas y puntuadores */
            if (t.lexeme) {
                free(t.lexeme);
            }

            switch (t.code) {
#define X(n) case TOK_##n: bison_token = T_##n; break;
                TOKEN_LIST(X)
#undef X
                default: bison_token = T_ERROR; break;
            }
            return bison_token;
    }
}


static void yyerror(const char *msg) {
    syntax_error_count++;
    fprintf(stderr, "Error de sintaxis: %s\n", msg);
}

int parse_file(
    const char *input_filename,
    int* syntax_errors
) {
    if (input_filename == NULL) return 0;

    if (!init_scanner(input_filename)) {
        fprintf(
            stderr,
            "Error: no se pudo abrir %s\n",
            input_filename
        );
        return 0;
    }

    syntax_error_count = 0;

    yyparse();
    close_scanner();

    if (syntax_errors) *syntax_errors = syntax_error_count;

    return syntax_error_count == 0;
}