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

/* Tabla de alias creados con typedef (nombres de tipo definidos por el usuario) */
static char **alias_names = NULL;
static int alias_count = 0;
static int alias_cap = 0;
static int declaring_alias = 0;  /* 1 mientras la declaracion actual lleva typedef */
static int inner_depth = 0;      /* > 0 dentro de parametros de funcion o de llaves de struct/union */

static int alias_known(const char *name) {
    for (int i = 0; i < alias_count; i++)
        if (strcmp(alias_names[i], name) == 0) return 1;
    return 0;
}

static void alias_register(const char *name) {
    if (alias_known(name)) return;
    if (alias_count == alias_cap) {
        alias_cap = alias_cap ? alias_cap * 2 : 16;
        alias_names = realloc(alias_names, alias_cap * sizeof(char *));
        if (!alias_names) { perror("realloc"); exit(1); }
    }
    alias_names[alias_count++] = strdup(name);
}

static void alias_reset(void) {
    for (int i = 0; i < alias_count; i++) free(alias_names[i]);
    free(alias_names);
    alias_names = NULL;
    alias_count = alias_cap = 0;
    declaring_alias = 0;
    inner_depth = 0;
}
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
%token DEFAULT DO DOUBLE ENUM EXTERN
%token FLOAT FOR GOTO IF INT LONG
%token REGISTER RETURN SHORT SIGNED SIZEOF
%token STATIC STRUCT SWITCH TYPEDEF UNION
%token UNSIGNED VOID VOLATILE WHILE
%token INLINE RESTRICT ALIGNAS ALIGNOF ATOMIC BOOL
%token COMPLEX GENERIC IMAGINARY NORETURN
%token STATIC_ASSERT THREAD_LOCAL

/* Identifiers and constants */
%token <sval> IDENTIFIER USER_TYPE STRING_LITERAL
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

/*Declara ELSE y le da mayor precedencia que a la alternativa if sin else*/
%precedence IF_WITHOUT_ELSE
%precedence ELSE

%start translation_unit

%%

/* Gramática de prueba para expresiones.
   Todavía NO reconoce programas C completos. */


/* Programa completo */
translation_unit:
      external_declaration
    | translation_unit external_declaration
    ;

/* Elementos permitidos a nivel global */
external_declaration:
      declaration
    | function_definition
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
    | PLUS cast_expr
    | MINUS cast_expr
    | NOT cast_expr
    | BIT_NOT cast_expr
    | STAR cast_expr
    | BIT_AND cast_expr
    | SIZEOF unary_expression
    | SIZEOF LPAREN type_label RPAREN
    ;

/* Conversion explicita de tipo: (tipo) expresion */
cast_expr:
      unary_expression
    | LPAREN type_label RPAREN cast_expr
    ;

/* Multiplicación, división y módulo */
multiplicative_expression:
      cast_expr
    | multiplicative_expression STAR cast_expr
    | multiplicative_expression SLASH cast_expr
    | multiplicative_expression PERCENT cast_expr
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


/* Tipos de datos basicos */
type_specifier:
      VOID
    | CHAR
    | SHORT
    | INT
    | LONG
    | FLOAT
    | DOUBLE
    | SIGNED
    | UNSIGNED
    | BOOL
    | COMPLEX
    | record_spec
    | enum_spec
    ;

/* struct y union */
record_spec:
      record_kind IDENTIFIER LBRACE { inner_depth++; } member_list RBRACE
          { inner_depth--; free($2); }
    | record_kind LBRACE { inner_depth++; } member_list RBRACE
          { inner_depth--; }
    | record_kind IDENTIFIER { free($2); }
    ;

record_kind:
      STRUCT
    | UNION
    ;

/* Miembros del registro */
member_list:
      member_decl
    | member_list member_decl
    ;

member_decl:
      member_specs member_declarator_list SEMICOLON
    | member_specs SEMICOLON
    ;

/* Tipo de un miembro (sin clases de almacenamiento) */
member_specs:
      type_specifier
    | user_type_ref
    | type_qualifier
    | type_specifier member_specs
    | user_type_ref member_specs
    | type_qualifier member_specs
    ;

member_declarator_list:
      member_declarator
    | member_declarator_list COMMA member_declarator
    ;

/* Miembro normal o campo de bits */
member_declarator:
      declarator
    | declarator COLON conditional_expression
    | COLON conditional_expression
    ;

/* enum */
enum_spec:
      ENUM IDENTIFIER LBRACE enum_item_list enum_comma_opt RBRACE { free($2); }
    | ENUM LBRACE enum_item_list enum_comma_opt RBRACE
    | ENUM IDENTIFIER { free($2); }
    ;

enum_comma_opt:
      %empty
    | COMMA
    ;

enum_item_list:
      enum_item
    | enum_item_list COMMA enum_item
    ;

enum_item:
      IDENTIFIER { free($1); }
    | IDENTIFIER ASSIGN conditional_expression { free($1); }
    ;

/* Calificadores de tipo */
type_qualifier:
      CONST
    | VOLATILE
    | RESTRICT
    ;

/* Nombre de tipo creado por el usuario con typedef */
user_type_ref:
      USER_TYPE { free($1); }
    ;

/* Palabra clave typedef: marca la declaracion como definicion de alias */
alias_keyword:
      TYPEDEF { declaring_alias = 1; }
    ;

/* Clases de almacenamiento y especificadores de funcion (sin typedef) */
storage_word:
      STATIC
    | EXTERN
    | AUTO
    | REGISTER
    | THREAD_LOCAL
    | INLINE
    | NORETURN
    ;

/* Especificadores de una declaracion */
declaration_specifiers:
      type_specifier
    | user_type_ref
    | type_qualifier declaration_specifiers
    | alias_keyword declaration_specifiers
    | storage_word declaration_specifiers
    | type_specifier declaration_specifiers
    | user_type_ref declaration_specifiers
    ;

/* Declaraciones */
declaration:
      declaration_specifiers SEMICOLON
          { declaring_alias = 0; }
    | declaration_specifiers init_declarator_list SEMICOLON
          { declaring_alias = 0; }
    ;

/* Lista de variables declaradas */
init_declarator_list:
      init_declarator
    | init_declarator_list COMMA init_declarator
    ;

/* Declarador con inicializacion opcional */
init_declarator:
      declarator
    | declarator ASSIGN initializer
    ;

/* Inicializador: expresion o lista entre llaves */
initializer:
      assignment_expression
    | LBRACE init_item_list RBRACE
    | LBRACE init_item_list COMMA RBRACE
    ;

init_item_list:
      init_item
    | init_item_list COMMA init_item
    ;

/* Elemento de la lista, con designador opcional: .campo = v, [i] = v */
init_item:
      initializer
    | designator_chain ASSIGN initializer
    ;

designator_chain:
      designator_step
    | designator_chain designator_step
    ;

designator_step:
      LBRACKET conditional_expression RBRACKET
    | DOT IDENTIFIER { free($2); }
    ;


/* Declarador */
declarator:
      direct_declarator
    | STAR pointer_opt direct_declarator
    ;

/* Punteros */
pointer_opt:
      %empty
    | STAR pointer_opt
    ;


/* Declaradores directos */
direct_declarator:
      IDENTIFIER
          {
              if (declaring_alias && inner_depth == 0) alias_register($1);
              free($1);
          }
    | LPAREN declarator RPAREN
    | direct_declarator LBRACKET array_size_opt RBRACKET
    | direct_declarator LPAREN { inner_depth++; }
      parameter_type_list_opt RPAREN { inner_depth--; }
    ;

/* Parametros opcionales de una funcion */
parameter_type_list_opt:
      %empty
    | parameter_list
    | parameter_list COMMA ELLIPSIS
    ;

/* Lista de parametros */
parameter_list:
      parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

/* Declaracion de cada parametro */
parameter_declaration:
      declaration_specifiers declarator
    | declaration_specifiers nameless_declarator
    | declaration_specifiers
    ;

/* Tipo completo sin nombre: usado en casts, sizeof y parametros anonimos */
type_label:
      member_specs
    | member_specs nameless_declarator
    ;

/* Declarador sin identificador */
nameless_declarator:
      STAR pointer_opt
    | STAR pointer_opt nameless_direct
    | nameless_direct
    ;

nameless_direct:
      LPAREN nameless_declarator RPAREN
    | LBRACKET array_size_opt RBRACKET
    | nameless_direct LBRACKET array_size_opt RBRACKET
    | LPAREN parameter_type_list_opt RPAREN
    | nameless_direct LPAREN parameter_type_list_opt RPAREN
    ;

/* Tamano de arreglo opcional */
array_size_opt:
      %empty
    | assignment_expression
    ;


/* Definicion de funciones */
function_definition:
      declaration_specifiers declarator compound_statement
    ;

/* Bloques delimitados por llaves */
compound_statement:
      LBRACE RBRACE
    | LBRACE block_item_list RBRACE
    ;

/* Lista de elementos dentro de un bloque */
block_item_list:
      block_item
    | block_item_list block_item
    ;

/* Un bloque puede contener declaraciones o sentencias */
block_item:
      declaration
    | statement
    ;

/* Sentencias */
statement:
      labeled_statement
    | compound_statement
    | expression_statement
    | selection_statement
    | iteration_statement
    | jump_statement
    ;

/* Sentencias de expresion */
expression_statement:
      SEMICOLON
    | expression SEMICOLON
    ;

/* Sentencias etiquetadas */
labeled_statement:
      IDENTIFIER COLON statement { free($1); }
    | CASE conditional_expression COLON statement
    | DEFAULT COLON statement
    ;


/* Condicionales */
selection_statement:
      IF LPAREN expression RPAREN statement
          %prec IF_WITHOUT_ELSE
    | IF LPAREN expression RPAREN statement ELSE statement
    | SWITCH LPAREN expression RPAREN statement
    ;

/* Ciclos */
iteration_statement:
      WHILE LPAREN expression RPAREN statement
    | DO statement WHILE LPAREN expression RPAREN SEMICOLON
    | FOR LPAREN expression_opt SEMICOLON expression_opt
      SEMICOLON expression_opt RPAREN statement
    | FOR LPAREN declaration expression_opt
      SEMICOLON expression_opt RPAREN statement
    ;

/* Expresion opcional */
expression_opt:
      %empty
    | expression
    ;

/* Sentencias de salto */
jump_statement:
      GOTO IDENTIFIER SEMICOLON { free($2); }
    | CONTINUE SEMICOLON
    | BREAK SEMICOLON
    | RETURN SEMICOLON
    | RETURN expression SEMICOLON
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
            yylval.sval = t.lexeme;
            return (t.lexeme && alias_known(t.lexeme)) ? T_USER_TYPE : T_IDENTIFIER;

        case TOK_STRING_LITERAL:
            yylval.sval = t.lexeme;
            return T_STRING_LITERAL;

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
    alias_reset();

    yyparse();
    close_scanner();
    alias_reset();

    if (syntax_errors) *syntax_errors = syntax_error_count;

    return syntax_error_count == 0;
}