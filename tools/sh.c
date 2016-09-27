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
static char buf[512];

int
initsh()
{
    FILE *tf, *uf;

    if (prepsupdir() != 0) {
        fputs("failed to prepare the Application Support Directory.\n",
              stderr);
        return -1;
    }
    if (genpath(buf, sizeof buf, "/fin") != 0)
        return -1;
    errno = 0;
    tf = fopen(buf, "w+b");
    if (tf == NULL) {
        perror("initsh");
        return -1;
    }
    uf = fopen(buf, "r+b");
    if (uf == NULL) {
        perror("initsh");
        fclose(tf);
        return -1;
    } else {
        origfin = stdin;
        stdin = tf;
        finout = uf;
        return 0;
    }
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