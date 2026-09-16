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

/* regex for literals & numbers */
DIGIT       [0-9]
HEX         [0-9a-fA-F]
LETTER      [a-zA-Z_]
IDENT       {LETTER}({LETTER}|{DIGIT})*

INT_DEC     [1-9]{DIGIT}*|0
INT_HEX     0[xX]{HEX}+
INT_OCT     0[0-7]+
FLOAT       ({DIGIT}+\.{DIGIT}*|\.{DIGIT}+)([eE][+-]?{DIGIT}+)?[fFlL]?|{DIGIT}+[eE][+-]?{DIGIT}+[fFlL]?

%%

[ \t\r]+    { col_num += yyleng; /* Ignore spaces */ }
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

{INT_DEC}|{INT_HEX}|{INT_OCT} { return set_token(TOK_INT_CONST, CAT_CONSTANT); }
{FLOAT}                       { return set_token(TOK_FLOAT_CONST, CAT_CONSTANT); }
\'([^\\\'\n]|\\.)+\'          { return set_token(TOK_CHAR_CONST, CAT_CONSTANT); }
\"([^\\\"\n]|\\.)*\"          { return set_token(TOK_STRING_LITERAL, CAT_STRING_LITERAL); }

{IDENT}     { return set_token(TOK_IDENTIFIER, CAT_IDENTIFIER); }

"->"        { return set_token(TOK_ARROW, CAT_OPERATOR); }
"++"        { return set_token(TOK_INCREMENT, CAT_OPERATOR); }
"--"        { return set_token(TOK_DECREMENT, CAT_OPERATOR); }
"=="        { return set_token(TOK_EQ, CAT_OPERATOR); }
"!="        { return set_token(TOK_NEQ, CAT_OPERATOR); }
"<="        { return set_token(TOK_LE, CAT_OPERATOR); }
">="        { return set_token(TOK_GE, CAT_OPERATOR); }
"&&"        { return set_token(TOK_AND, CAT_OPERATOR); }
"||"        { return set_token(TOK_OR, CAT_OPERATOR); }
"="         { return set_token(TOK_ASSIGN, CAT_OPERATOR); }
"+"         { return set_token(TOK_PLUS, CAT_OPERATOR); }
"-"         { return set_token(TOK_MINUS, CAT_OPERATOR); }
"*"         { return set_token(TOK_STAR, CAT_OPERATOR); }
"/"         { return set_token(TOK_SLASH, CAT_OPERATOR); }
"%"         { return set_token(TOK_PERCENT, CAT_OPERATOR); }
"!"         { return set_token(TOK_NOT, CAT_OPERATOR); }

"{"         { return set_token(TOK_LBRACE, CAT_PUNCTUATOR); }
"}"         { return set_token(TOK_RBRACE, CAT_PUNCTUATOR); }
"("         { return set_token(TOK_LPAREN, CAT_PUNCTUATOR); }
")"         { return set_token(TOK_RPAREN, CAT_PUNCTUATOR); }
"["         { return set_token(TOK_LBRACKET, CAT_PUNCTUATOR); }
"]"         { return set_token(TOK_RBRACKET, CAT_PUNCTUATOR); }
";"         { return set_token(TOK_SEMICOLON, CAT_PUNCTUATOR); }
","         { return set_token(TOK_COMMA, CAT_PUNCTUATOR); }
"."         { return set_token(TOK_DOT, CAT_PUNCTUATOR); }

.           { return set_token(TOK_ERROR, CAT_ERROR); }
<<EOF>>     { return set_token(TOK_EOF, CAT_EOF); }

%%