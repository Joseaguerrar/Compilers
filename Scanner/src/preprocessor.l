%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>

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

    static void print_macros() {
        for (int i = 0; i < macro_count; i++) {
            printf("\n%s\n%s\n%d\n", macro_table[i].key,macro_table[i].value, macro_table[i].type );
        }
    }

    static int macro_insert(const char* key, const char* value, const MacroType type) {
        if (macro_count >= MAX_MACROS) {
          fprintf(stderr, "Error: tabla de macros llena\n");
          return 0;
      }
        
        for (int i = 0; i < macro_count; i++) {
            if (strcmp(macro_table[i].key, key) == 0) {
                fprintf(stderr, "Error: Macro %s ya definida", key);
                return 0;
            }
        }

        macro_table[macro_count].key = strdup(key);
        macro_table[macro_count].value = strdup(value);
        macro_table[macro_count].type = type;

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
            value_buffer[value_index] = *temp;
            value_index++;
            temp++;
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
%}

/* Estado para comentarios multilínea */
%x COMMENT

%%
^[ \t]*#[ \t]*define[ \t]+[A-Za-z_][A-Za-z0-9_]*[^\n]* {
    macro_handle_define(yytext);
}

"//"[^\n]*          {;}

"/*"                { BEGIN(COMMENT); }
<COMMENT>"*/"       { BEGIN(INITIAL); }
<COMMENT>\n         {;}
<COMMENT>.          {;}

<COMMENT><<EOF>>    {
    fprintf(stderr, "Error lexico: comentario sin cerrar\n");
    return 0;
}
%%

int yywrap(){
    return 1;
}

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

    print_macros();

    return 0;
}