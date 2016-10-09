%{
    #include "sh.h"
    #include <stdio.h>
    #include <string.h>
    extern int yylex();
    extern int yyerror(const char *s);
%}

%start list

%union {
    char s[80];
}

%token <s> WORD 256

%%

blank   : ' '
        | '\t'
        | ' ' blank
        | '\t' blank
        ;

word : WORD {
                if (shwltail == 0) {
                    shwltail = malloc(sizeof *shwltail);
                    shwltail->p = strdup($<s>1);
                    shwltail->next = shwltail;
                } else {
                    struct Wl *p;
                    p = malloc(sizeof *p);
                    p->p = strdup($<s>1);
                    p->next = shwltail->next;
                    shwltail->next = p;
                    shwltail = p;
                }
            }
        ;

simple  : '\n'
        | blank simple
        | word simple
        ;

list    : simple {
                if (!shwltail)
                    return 0;
                return 0;
            }
        | list simple
        ;

%%

int yyerror(const char *s)
{
    fprintf(stderr, "%s\n", s);
    return 0;
}
