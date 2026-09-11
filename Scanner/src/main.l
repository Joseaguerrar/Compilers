/*** Definitions can be accessed from yylex() or main() ***/
%{

    int count = 0;

%}

%%
[0-9] {count++;}
[A-zA-z] {continue;}
%%

int yywrap(){}

int main() {

    FILE *fp;
    char filename[50];
    printf("Enter the filename: \n");
    scanf("%s",filename);
    fp = fopen(filename,"r");
    yyin = fp;

    yylex();

    printf("\nNumber of numbers: %d\n", count);

    return 0;

}