//
//  sh.c
//  tools
//
//  Created by Yutaka sugawara on 2016/09/10.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "sh.h"
#include "impl.h"
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

struct Wl *shwltail;

static FILE *origfin;
static FILE *finout;
static FILE *origfout;
static FILE *foutin;
static char buf[512];

int
initsh()
{
    int e;
    FILE *tf, *uf;

    if (prepsupdir() != 0) {
        fputs("failed to prepare the Application Support Directory.\n",
              stderr);
        return -1;
    }
    e = 1;
    errno = 0;
    if (genpath(buf, sizeof buf, "/fin") != 0)
        ;
    else if ((tf = fopen(buf, "w+b")) == NULL)
        perror("initsh:input file(out)");
    else if ((uf = fopen(buf, "r+b")) == NULL) {
        perror("initsh:input file(in)");
        fclose(tf);
    } else {
        origfin = stdin;
        stdin = uf;
        finout = tf;
        e = 0;
    }
    if (e)
        return -1;
    e = 1;
    if (genpath(buf, sizeof buf, "/fout") != 0)
        ;
    else if ((tf = fopen(buf, "w+b")) == NULL)
        perror("initsh:output file(out)");
    else if ((uf = fopen(buf, "r+b")) == NULL) {
        perror("initsh:output file(in)");
        fclose(tf);
    } else {
        origfout = stdout;
        stdout = tf;
        foutin = uf;
        e = 0;
    }
    if (e) {
        cleanupsh();
        return -1;
    } else
        return 0;
}

void
cleanupsh()
{
    if (finout) {
        fclose(finout);
        finout = 0;
    }
    if (origfin) {
        if (stdin)
            fclose(stdin);
        stdin = origfin;
    }
    if (foutin) {
        fclose(foutin);
        foutin = 0;
    }
    if (origfout) {
        if (stdout)
            fclose(stdout);
        stdout = origfout;
    }
}

int
putsonin(const char *s)
{
    if (fputs(s, finout)) {
        fflush(finout);
        return 0;
    } else
        return -1;
}

int
getsfromin(char *b, int bsz)
{
    if (fgets(b, bsz, stdin))
        return 0;
    else
        return -1;
}

int
putsonout(const char *s)
{
    if (fputs(s, stdout)) {
        fflush(stdout);
        return 0;
    } else
        return -1;
}

int
getsfromout(char *b, int bsz)
{
    if (fgets(b, bsz, foutin))
        return 0;
    else
        return -1;
}

void
procin()
{
    extern int true_main(int, char**);
    extern int seq_main(int, char **);
    char **argv;
    int i;
    
    clearerr(stdin);
    yyparse();
    
    /* just to link */
    /*true_main(0, 0);*/
    argv = malloc(sizeof(*argv) * 4);
    argv[0] = strdup("seq");
    argv[1] = strdup("1");
    argv[2] = strdup("10");
    argv[3] = 0;
    seq_main(3, argv);
    for (i = 0; i < 3; ++i)
        free(argv[i]);
    free(argv);
}
