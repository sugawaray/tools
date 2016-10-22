//
//  io_c.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/22.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <sh.h>
#include <stdio.h>
#include <string.h>

FILE *
convfp(FILE *(*fps)[2], FILE *fp)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (fps[i][0] == fp)
            break;
    }
    if (i < 3)
        return fps[i][1];
    else
        return fp;
}

int
convfd(FILE *(*fps)[2], int fd)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (fileno(fps[i][0]) == fd)
            break;
    }
    if (i < 3)
        return fileno(fps[i][1]);
    else
        return fd;
}

void
procioinit(struct Procio *o, FILE *pin, FILE *pout, FILE *perr)
{
#undef ASSIGN
#define ASSIGN(d, s)    d[0] = s; d[1] = s;
    ASSIGN(o->fp[0], pin)
    ASSIGN(o->fp[1], pout)
    ASSIGN(o->fp[2], perr)
    memset(o->redir, 0, sizeof o->redir);
}
