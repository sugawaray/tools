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
        | word simple
        ;

list    : simple {
                int i;
                struct Wl *p, *q;

                if (!shwltail)
                    return 0;
                i = 0;
                p = shwltail->next;
                do {
                    printf("[%d: %s]\n", i, p->p);
                    ++i;
                    p = p->next;
                } while (p != shwltail->next);
                p = shwltail->next;
                shwltail->next = 0;
                while (p) {
                    q = p;
                    p = p->next;
                    if (q) {
                        free(q->p);
                        free(q);
                    }
                }
                shwltail = 0;
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