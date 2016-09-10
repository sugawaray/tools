%{
    int yywrap();
%}

WORD    [^|&;<>() \t\n]+

%%
{WORD}  {
            printf("[%s]\n", yytext);
        }

%%
int yywrap()
{
    return 1;
}