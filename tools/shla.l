%{
    #include "shya.tab.h"
    int yywrap();
%}

WORD    [^|&;<>() \t\n]+
LIT [\n]

%%
{WORD}  {
            printf("[%s]\n", yytext);
            strcpy(yylval.s, yytext);
            return WORD;
        }

{LIT}   {
            return yytext[0];
        }

%%
int yywrap()
{
    return 1;
}