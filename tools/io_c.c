//
//  io_c.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/22.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <sh.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *
procioconvfp(struct Procio *o, FILE *fp)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (o->fp[i] == fp)
            break;
    }
    if (i < 3)
        return o->pipe[i]->fp[0];
    else
        return fp;
}

int
procioconvfd(struct Procio *o, int fd)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (fileno(o->fp[i]) == fd)
            break;
    }
    if (i < 3) {
        if (o->pipe[i]->fp[0])
            return fileno(o->pipe[i]->fp[0]);
        else
            return -1;
    } else
        return fd;
}

int
prociofpidx(struct Procio *o, FILE *fp)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (o->fp[i] == fp)
            break;
    }
    if (i < 3)
        return i;
    else
        return -1;
}

int
prociofdidx(struct Procio *o, int fd)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (fileno(o->fp[i]) == fd)
            break;
    }
    if (i < 3)
        return i;
    else
        return -1;
}

void
procioinit(struct Procio *o, FILE *pin, FILE *pout, FILE *perr)
{
    o->fp[0] = pin;
    o->fp[1] = pout;
    o->fp[2] = perr;
    o->bk = 0;
    memset(o->pipe, 0, sizeof o->pipe);
    o->iotail = 0;
}

struct Pipe *
procioallocio(struct Procio *o)
{
    struct Pipeelem *p;
    p = malloc(sizeof(struct Pipeelem));
    memset(p, 0, sizeof *p);
    p->o.fd[0] = p->o.fd[1] = -1;
    if (o->iotail) {
        p->next = o->iotail->next;
        o->iotail->next = p;
    } else
        p->next = p;
    o->iotail = p;
    return &o->iotail->o;
}

void
prociofreeio(struct Procio *o)
{
    struct Pipeelem *head, *p, *q;
    if (!o->iotail)
        return;
    head = p = o->iotail->next;
    while (p) {
        q = p->next;
        free(p);
        if (q == head)
            break;
        p = q;
    }
}

