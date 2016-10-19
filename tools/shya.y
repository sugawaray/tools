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

cmd_word    : WORD {
                    if (shwltail == 0) {
                        shwltail = malloc(sizeof *shwltail);
                        shwltail->p = strdup($<s>1);
                        shwltail->next = shwltail;
                    } else {
                        dbgput2("unexpected(cmd_word1)\n");
                    }
                }
            | cmd_word blank WORD {
                    if (shwltail) {
                        struct Wl *p;
                        p = malloc(sizeof *p);
                        p->p = strdup($<s>3);
                        p->next = shwltail->next;
                        shwltail->next = p;
                        shwltail = p;
                    } else {
                        dbgput2("unexpected(cmd_word2)\n");
                    }
                }
            | cmd_word WORD {
                    if (shwltail) {
                        struct Wl *p;
                        p = malloc(sizeof *p);
                        p->p = strdup($<s>2);
                        p->next = shwltail->next;
                        shwltail->next = p;
                        shwltail = p;
                    } else {
                        dbgput2("unexpected(cmd_word3)\n");
                    }
                }
            ;

io_redirect : '>' WORD {
                    if (redirp)
                        free(redirp);
                    redirp = strdup($<s>2);
                }
            ;

simple_command  : cmd_word
                | cmd_word io_redirect
                | cmd_word blank io_redirect
                ;

simple  : '\n'
        | simple_command simple
        | blank simple
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
