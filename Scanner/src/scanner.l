%{
#include "token.h"
#include <string.h>
#include <stdlib.h>

Token current_token;
int line_num = 1;
int col_num = 1;

static int set_token(TokenType type, TokenCategory cat) {
    current_token.code = type;
    current_token.category = cat;
    current_token.lexeme = strdup(yytext);
    current_token.line = line_num;
    current_token.column = col_num;
    col_num += yyleng;
    return type;
}
%}

%option noyywrap
%option nounput noinput

%option prefix="sc_"

DIGIT       [0-9]
HEX         [0-9a-fA-F]
OCTAL       [0-7]

LETTER      [a-zA-Z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*

/* Integer constants */
INT_DEC     [1-9]{DIGIT}*
INT_HEX     0[xX]{HEX}+
INT_OCT     0{OCTAL}*

USUFF       [uU]
LSUFF       [lL]
LLSUFF      (ll|LL)

INT_SUFFIX  ({USUFF}({LLSUFF}|{LSUFF})?|({LLSUFF}|{LSUFF}){USUFF}?)

/* Decimal floating constants */
FLOAT       ({DIGIT}+\.{DIGIT}*|\.{DIGIT}+)([eE][+-]?{DIGIT}+)?[fFlL]?|{DIGIT}+[eE][+-]?{DIGIT}+[fFlL]?

/* Hexadecimal floating constants */
FLOAT_SUFFIX    [fFlL]
BIN_EXP         [pP][+-]?{DIGIT}+
HEX_FRAC        ({HEX}+\.{HEX}*|\.{HEX}+)
HEX_FLOAT       0[xX](({HEX_FRAC}|{HEX}+){BIN_EXP}){FLOAT_SUFFIX}?

BAD_OCT         0[0-7]*[89][0-9]*
BAD_HEX         0[xX]
BAD_EXP_INT     {DIGIT}+[eE][+-]?
BAD_EXP_FLOAT   ({DIGIT}+\.{DIGIT}*|\.{DIGIT}+)[eE][+-]?

/*Literal prefixes*/
CHAR_PREFIX     (L|u|U)
STRING_PREFIX   (u8|u|U|L)

/* Valid C escape sequences */
ESC_SIMPLE  \\['"?\\abfnrtv]
ESC_OCTAL   \\{OCTAL}{1,3}
ESC_HEX     \\x{HEX}+
ESC_UCN     (\\u{HEX}{4}|\\U{HEX}{8})

ESCAPE      ({ESC_SIMPLE}|{ESC_OCTAL}|{ESC_HEX}|{ESC_UCN})

/* Valid characters inside literals */
STRING_CHAR ([^\\\"\n]|{ESCAPE})
CHAR_CHAR   ([^\\'\n]|{ESCAPE})

PP_NUMBER   ({DIGIT}|\.{DIGIT})([A-Za-z_0-9.]|[eEpP][+-])*

%%

[ \t\r]+    { col_num += yyleng; }
\n          { line_num++; col_num = 1; }

"auto"      { return set_token(TOK_AUTO, CAT_KEYWORD); }
"break"     { return set_token(TOK_BREAK, CAT_KEYWORD); }
"case"      { return set_token(TOK_CASE, CAT_KEYWORD); }
"char"      { return set_token(TOK_CHAR, CAT_KEYWORD); }
"const"     { return set_token(TOK_CONST, CAT_KEYWORD); }
"continue"  { return set_token(TOK_CONTINUE, CAT_KEYWORD); }
"default"   { return set_token(TOK_DEFAULT, CAT_KEYWORD); }
"do"        { return set_token(TOK_DO, CAT_KEYWORD); }
"double"    { return set_token(TOK_DOUBLE, CAT_KEYWORD); }
"else"      { return set_token(TOK_ELSE, CAT_KEYWORD); }
"enum"      { return set_token(TOK_ENUM, CAT_KEYWORD); }
"extern"    { return set_token(TOK_EXTERN, CAT_KEYWORD); }
"float"     { return set_token(TOK_FLOAT, CAT_KEYWORD); }
"for"       { return set_token(TOK_FOR, CAT_KEYWORD); }
"goto"      { return set_token(TOK_GOTO, CAT_KEYWORD); }
"if"        { return set_token(TOK_IF, CAT_KEYWORD); }
"int"       { return set_token(TOK_INT, CAT_KEYWORD); }
"long"      { return set_token(TOK_LONG, CAT_KEYWORD); }
"register"  { return set_token(TOK_REGISTER, CAT_KEYWORD); }
"return"    { return set_token(TOK_RETURN, CAT_KEYWORD); }
"short"     { return set_token(TOK_SHORT, CAT_KEYWORD); }
"signed"    { return set_token(TOK_SIGNED, CAT_KEYWORD); }
"sizeof"    { return set_token(TOK_SIZEOF, CAT_KEYWORD); }
"static"    { return set_token(TOK_STATIC, CAT_KEYWORD); }
"struct"    { return set_token(TOK_STRUCT, CAT_KEYWORD); }
"switch"    { return set_token(TOK_SWITCH, CAT_KEYWORD); }
"typedef"   { return set_token(TOK_TYPEDEF, CAT_KEYWORD); }
"union"     { return set_token(TOK_UNION, CAT_KEYWORD); }
"unsigned"  { return set_token(TOK_UNSIGNED, CAT_KEYWORD); }
"void"      { return set_token(TOK_VOID, CAT_KEYWORD); }
"volatile"  { return set_token(TOK_VOLATILE, CAT_KEYWORD); }
"while"     { return set_token(TOK_WHILE, CAT_KEYWORD); }
"inline"    { return set_token(TOK_INLINE, CAT_KEYWORD); }
"restrict"  { return set_token(TOK_RESTRICT, CAT_KEYWORD); }

"_Alignas"  { return set_token(TOK_ALIGNAS, CAT_KEYWORD); }
"_Alignof"  { return set_token(TOK_ALIGNOF, CAT_KEYWORD); }
"_Atomic"   { return set_token(TOK_ATOMIC, CAT_KEYWORD); }
"_Bool"     { return set_token(TOK_BOOL, CAT_KEYWORD); }
"_Complex"  { return set_token(TOK_COMPLEX, CAT_KEYWORD); }
"_Generic"  { return set_token(TOK_GENERIC, CAT_KEYWORD); }
"_Imaginary" { return set_token(TOK_IMAGINARY, CAT_KEYWORD); }
"_Noreturn" { return set_token(TOK_NORETURN, CAT_KEYWORD); }
"_Static_assert" { return set_token(TOK_STATIC_ASSERT, CAT_KEYWORD); }
"_Thread_local" { return set_token(TOK_THREAD_LOCAL, CAT_KEYWORD); }

 /* malformed numeric literals */
{BAD_OCT} {return set_token(TOK_ERROR, CAT_ERROR);}

{BAD_HEX} {return set_token(TOK_ERROR, CAT_ERROR);}

{BAD_EXP_FLOAT} {return set_token(TOK_ERROR, CAT_ERROR);}

{BAD_EXP_INT} {return set_token(TOK_ERROR, CAT_ERROR);}

{HEX_FLOAT} {return set_token(TOK_FLOAT_CONST, CAT_CONSTANT);}

({INT_HEX}|{INT_DEC}|{INT_OCT})({INT_SUFFIX})? {return set_token(TOK_INT_CONST, CAT_CONSTANT);}

{FLOAT} {return set_token(TOK_FLOAT_CONST, CAT_CONSTANT);}

{PP_NUMBER} {
    return set_token(TOK_ERROR, CAT_ERROR);
}

 /* Valid character constant */
({CHAR_PREFIX})?\'{CHAR_CHAR}+\' {
    return set_token(
        TOK_CHAR_CONST,
        CAT_CONSTANT
    );
}

 /* Empty character constant */
({CHAR_PREFIX})?\'\' {
    return set_token(
        TOK_ERROR,
        CAT_ERROR
    );
}

 /* Invalid but closed character constant */
({CHAR_PREFIX})?\'([^\\'\n]|\\.)+\' {
    return set_token(
        TOK_ERROR,
        CAT_ERROR
    );
}

 /* Unterminated character constant */
({CHAR_PREFIX})?\'([^\\'\n]|\\.)* {
    return set_token(
        TOK_ERROR,
        CAT_ERROR
    );
}


 /* Valid string literal */
({STRING_PREFIX})?\"{STRING_CHAR}*\" {
    return set_token(
        TOK_STRING_LITERAL,
        CAT_STRING_LITERAL
    );
}

 /* Invalid escape inside a closed string */
({STRING_PREFIX})?\"([^\\\"\n]|\\.)*\" {
    return set_token(
        TOK_ERROR,
        CAT_ERROR
    );
}

 /* Unterminated string */
({STRING_PREFIX})?\"([^\\\"\n]|\\.)* {
    return set_token(
        TOK_ERROR,
        CAT_ERROR
    );
}

{IDENT}     { return set_token(TOK_IDENTIFIER, CAT_IDENTIFIER); }

"->"        { return set_token(TOK_ARROW, CAT_OPERATOR); }
"++"        { return set_token(TOK_INCREMENT, CAT_OPERATOR); }
"--"        { return set_token(TOK_DECREMENT, CAT_OPERATOR); }
">"         { return set_token(TOK_GT, CAT_OPERATOR); }
"<"         { return set_token(TOK_LT, CAT_OPERATOR); }
"=="        { return set_token(TOK_EQ, CAT_OPERATOR); }
"!="        { return set_token(TOK_NEQ, CAT_OPERATOR); }
"<="        { return set_token(TOK_LE, CAT_OPERATOR); }
">="        { return set_token(TOK_GE, CAT_OPERATOR); }
"&&"        { return set_token(TOK_AND, CAT_OPERATOR); }
"&"         { return set_token(TOK_BIT_AND, CAT_OPERATOR); }
"&="        { return set_token(TOK_BIT_AND_ASSIGN, CAT_OPERATOR); }
"||"        { return set_token(TOK_OR, CAT_OPERATOR); }
"|"         { return set_token(TOK_BIT_OR, CAT_OPERATOR); }
"|="        { return set_token(TOK_BIT_OR_ASSIGN, CAT_OPERATOR); }
"^"         { return set_token(TOK_BIT_XOR, CAT_OPERATOR); }
"^="        { return set_token(TOK_BIT_XOR_ASSIGN, CAT_OPERATOR); }
"~"         { return set_token(TOK_BIT_NOT, CAT_OPERATOR); }
"<<"        { return set_token(TOK_LSHIFT, CAT_OPERATOR); }
">>"        { return set_token(TOK_RSHIFT, CAT_OPERATOR); }
"<<="       { return set_token(TOK_LSHIFT_ASSIGN, CAT_OPERATOR); }
">>="       { return set_token(TOK_RSHIFT_ASSIGN, CAT_OPERATOR); }
"="         { return set_token(TOK_ASSIGN, CAT_OPERATOR); }
"+"         { return set_token(TOK_PLUS, CAT_OPERATOR); }
"+="        { return set_token(TOK_PLUS_ASSIGN, CAT_OPERATOR); }
"-"         { return set_token(TOK_MINUS, CAT_OPERATOR); }
"-="        { return set_token(TOK_MINUS_ASSIGN, CAT_OPERATOR); }
"*"         { return set_token(TOK_STAR, CAT_OPERATOR); }
"*="        { return set_token(TOK_STAR_ASSIGN, CAT_OPERATOR); }
"/"         { return set_token(TOK_SLASH, CAT_OPERATOR); }
"/="        { return set_token(TOK_SLASH_ASSIGN, CAT_OPERATOR); }
"%"         { return set_token(TOK_PERCENT, CAT_OPERATOR); }
"%="        { return set_token(TOK_PERCENT_ASSIGN, CAT_OPERATOR); }
"!"         { return set_token(TOK_NOT, CAT_OPERATOR); }

"{"         { return set_token(TOK_LBRACE, CAT_PUNCTUATOR); }
"}"         { return set_token(TOK_RBRACE, CAT_PUNCTUATOR); }
"("         { return set_token(TOK_LPAREN, CAT_PUNCTUATOR); }
")"         { return set_token(TOK_RPAREN, CAT_PUNCTUATOR); }
"["         { return set_token(TOK_LBRACKET, CAT_PUNCTUATOR); }
"]"         { return set_token(TOK_RBRACKET, CAT_PUNCTUATOR); }
";"         { return set_token(TOK_SEMICOLON, CAT_PUNCTUATOR); }
":"         { return set_token(TOK_COLON, CAT_PUNCTUATOR); }
"?"         { return set_token(TOK_QUESTION, CAT_PUNCTUATOR); }
","         { return set_token(TOK_COMMA, CAT_PUNCTUATOR); }
"..."       { return set_token(TOK_ELLIPSIS, CAT_PUNCTUATOR); }
"."         { return set_token(TOK_DOT, CAT_PUNCTUATOR); }
"##"        { return set_token(TOK_DOUBLE_HASH, CAT_PUNCTUATOR); }
"#"         { return set_token(TOK_HASH, CAT_PUNCTUATOR); }
"%:%:"      { return set_token(TOK_DOUBLE_HASH, CAT_PUNCTUATOR); }

"<:"        { return set_token(TOK_LBRACKET, CAT_PUNCTUATOR); }
":>"        { return set_token(TOK_RBRACKET, CAT_PUNCTUATOR); }

"<%"        { return set_token(TOK_LBRACE, CAT_PUNCTUATOR); }
"%>"        { return set_token(TOK_RBRACE, CAT_PUNCTUATOR); }

"%:"        { return set_token(TOK_HASH, CAT_PUNCTUATOR); }

.           { return set_token(TOK_ERROR, CAT_ERROR); }
<<EOF>>     { return set_token(TOK_EOF, CAT_EOF); }

%%