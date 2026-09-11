%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

    #define MAX_MACROS 1000
    #define MAX_KEY 128
    #define MAX_VALUE 1024
    #define MAX_DEPTH 32

    typedef enum {
        M_INT,
        M_DOUBLE,
        M_STRING,
        M_CHAR,
        M_IDENT,
        M_EXPR,
        M_EMPTY
    } MacroType;

    static const char* TYPE_NAME[] = 
    {"int", "double", "string", "char", "ident", "expr", "empty"};

    typedef struct {
        char* key;
        char* val;
        MacroType type;
    } Macro;

    static Macro macro_table[MAX_MACROS];
    static int macro_count = 0;

    static void macro_insert(const char* key, const char* value) {

    }
%}

%%
^[ \t]*#[ \t]*define[ \t]+[A-Za-z_][A-Za-z0-9_]{0,31}    { printf("%d", 1); }

^[ \t]*"/*"([^*]|\*+[^*/])*\*+"/"       {;}
^[ \t]*\/\/.*   {;}
%%

int yywrap(){}

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
    scanf("%s",filename);
    fp = fopen(filename,"r");
    yyin = fp;

    yylex();
    
    fclose(fp);

    return 0;
}