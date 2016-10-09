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

extern int ls_main(int, char**);
extern int mkdir_main(int, char**);
extern int true_main(int, char**);
extern int seq_main(int, char **);

struct Wl *shwltail;
struct Shcmd shcmdtab[] = {
    { "true", true_main },
    { "seq", seq_main },
    { "ls", ls_main },
    { "mkdir", mkdir_main },
    { 0, 0 }
};

struct Iofile {
    int origfd;
    int fdalt;
    FILE *fpalt;
    FILE **fp;
    const char *name;
    int fd;
    const char *flg;
};

static FILE *origfin;
static FILE *finout;
static FILE *origfout;
static FILE *foutin;
static struct Iofile ioferr = {
    .origfd = -1,
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/ferr",
    .fd = STDERR_FILENO,
    .flg    = "wb"
};
static char buf[512];

static void
cleanio(struct Iofile *o)
{
    if (o->fpalt) {
        fclose(o->fpalt);
        o->fpalt = 0;
        o->fdalt = -1;
    }
    if (o->origfd != -1) {
        if (*o->fp)
            fclose(*o->fp);
        *o->fp = 0;
        if (dup2(o->origfd, o->fd) != -1)
            *o->fp = fdopen(o->fd, o->flg);
        close(o->origfd);
        o->origfd = -1;
    }
    o->fpalt = 0;
    o->fdalt = -1;
    o->fp = 0;
    o->origfd = -1;
}

static int
initio(struct Iofile *o, FILE **fp)
{
    int oflags;
    if (genpath(buf, sizeof buf, o->name) != 0)
        return -1;
    o->fp = fp;
    o->origfd = dup(STDERR_FILENO);
    if (o->origfd == -1)
        return -1;
    fclose(*fp);
    oflags = O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK;
    if (openat(o->fd, buf, oflags, S_IRWXU) == -1) {
        cleanio(o);
        return -1;
    }
    *o->fp = fdopen(STDERR_FILENO, "wb");
    if (!(*fp)) {
        cleanio(o);
        return -1;
    }
    o->fdalt = open(buf, O_RDONLY | O_NONBLOCK);
    if (o->fdalt == -1) {
        cleanio(o);
        return -1;
    }
    o->fpalt = fdopen(o->fdalt, "rb");
    if (!o->fpalt) {
        cleanio(o);
        return -1;
    }
    fseek(o->fpalt, 0, SEEK_END);
    return 0;
}

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
    }
    if (initio(&ioferr, &stderr) == -1) {
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
    cleanio(&ioferr);
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
    clearerr(foutin);
    if (fgets(b, bsz, foutin))
        return 0;
    else
        return -1;
}

int
putsonerr(const char *s)
{
    if (fputs(s, stderr)) {
        fflush(stderr);
        return 0;
    } else
        return -1;
}

int
getsfromerr(char *b, int bsz)
{
    clearerr(ioferr.fpalt);
    if (fgets(b, bsz, ioferr.fpalt))
        return 0;
    else
        return -1;
}

static void
freeshwl()
{
    struct Wl *p, *q;
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
}

void
procin()
{
    int argc;
    char **argv, **bkargv;
    const struct Shcmd *cmd;
    int i, r;
    size_t asz;
    extern const char *applet_name;
    
    clearerr(stdin);
    yyparse();

    if (!shwltail)
        return;
    r = genargv(shwltail->next, shwltail->next, &argv, &argc);
    bkargv = 0;
    if (r == 0 && argc > 0) {
        asz = sizeof(char*) * argc;
        bkargv = malloc(asz);
        if (bkargv)
            memcpy(bkargv, argv, asz);
        cmd = findcmd(argv[0]);
        if (cmd) {
            clearerr(stdout);
            clearerr(stderr);
            applet_name = argv[0];
            cmd->f(argc, argv);
            fflush(stdout);
            fflush(stderr);
        }
    }
    if (r == 0 && argv) {
        if (bkargv)
            for (i = 0; i < argc; ++i)
                free(bkargv[i]);
        free(argv);
        free(bkargv);
    }
    freeshwl();
}

int
genargv(struct Wl *p1, struct Wl *p2, char ***av, int *cp)
{
    int i, c;
    struct Wl *p;
    i = 0;
    p = p1;
    do {
        ++i;
        p = p->next;
    } while (p != p2);
    c = i;
    if (cp)
        *cp = c;
    if (av) {
        *av = malloc(sizeof(char*) * (c + 1));
        for (i = 0, p = p1; i < c; p = p->next, ++i)
            (*av)[i] = strdup(p->p);
        (*av)[i] = 0;
    }
    return 0;
}

const struct Shcmd *
findcmd(const char *name)
{
    struct Shcmd *p = shcmdtab;
    while (p->name && strcmp(name, p->name) != 0)
        ++p;
    return p;
}

void
dbgput(const char *fmt, va_list ap)
{
    if (origfout) {
        vfprintf(origfout, fmt, ap);
        fflush(origfout);
    } else {
        vfprintf(stdout, fmt, ap);
        fflush(stdout);
    }
}

void
dbgput2(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    dbgput(fmt, ap);
    va_end(ap);
}
