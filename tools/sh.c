//
//  sh.c
//  tools
//
//  Created by Yutaka sugawara on 2016/09/10.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "sh.h"
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

struct Wl *shwltail;

static FILE *origfin;
static FILE *finout;
static char pbuf[1024];

static
int
genpath(char *buf, int bsz, const char *suffix)
{
    extern CFURLRef getlibdir();
    size_t l, l2;
    CFURLRef u = getlibdir();
    if (!CFURLGetFileSystemRepresentation(u, false,
                                          (UInt8 *)buf,
                                          bsz))
        return -1;
    l = strlen(buf);
    if (l >= bsz - 1)
        return -1;
    l2 = strlen(suffix);
    if (bsz - l <= l2)
        return -1;
    strcat(buf, suffix);
    return 0;
}

static
void
prepsupdir()
{
    int r;
    r = genpath(pbuf, sizeof pbuf, "");
    if (r != 0) {
        fputs("genpath failed\n", stderr);
        return;
    }
    mkdir(pbuf, 0700);
}

int
initsh()
{
    FILE *tf, *uf;

    prepsupdir();
    if (genpath(pbuf, sizeof pbuf, "/fin") != 0)
        return -1;
    errno = 0;
    tf = fopen(pbuf, "w+b");
    if (tf == NULL) {
        perror("initsh");
        return -1;
    }
    uf = fopen(pbuf, "r+b");
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
    yyparse();
}