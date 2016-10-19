%{
    #include "shya.tab.h"
    int yywrap();
%}

WORD    [^|&;<>() \t\n]+
LIT [> \t\n]

%%
{WORD}  {
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
