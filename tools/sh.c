//
//  sh.c
//  tools
//
//  Created by Yutaka sugawara on 2016/09/10.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "impl.h"
#include <sh.h>
#include <CoreFoundation/CoreFoundation.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

extern int cat_main(int, char**);
extern int ed_main(int, char**);
extern int gzip_main(int, char **);
extern int ls_main(int, char**);
extern int mkdir_main(int, char**);
extern int pwd_main(int, char**);
extern int true_main(int, char**);
extern int seq_main(int, char **);
extern int cp_main(int, char **);
extern int rm_main(int, char **);
extern int tar_main(int, char **);
extern int grep_main(int, char **);

struct Wl *shwltail;
char *redirp;
struct Shcmd shcmdtab[] = {
    { "true", true_main },
    { "seq", seq_main },
    { "ls", ls_main },
    { "mkdir", mkdir_main },
    { "pwd", pwd_main },
    { "cd", cd_main },
    { "cat", cat_main },
    { "ed", ed_main },
    { "gzip", gzip_main },
    { "cp", cp_main },
    { "rm", rm_main },
    { "tar", tar_main },
    { "grep", grep_main },
    { 0, 0 }
};

struct Pipe iofin = {
    .fd = { -1, -1 },
    .fp = { 0, 0 },
    .name = "/fin",
    .dir = PipeDirRx
};

struct Pipe iofout = {
    .fd = { -1, -1 },
    .fp = { 0, 0 },
    .name = "/fout",
    .dir = PipeDirTx
};

struct Pipe ioferr = {
    .fd = { -1, -1 },
    .fp = { 0, 0 },
    .name = "/ferr",
    .dir = PipeDirTx
};

static char buf[512];
struct Procio procio;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void
closeio(struct Pipe *o)
{
    int i;
    pthread_mutex_lock(&mut);
    for (i = 0; i < 2; ++i) {
        if (o->fp[i]) {
            fclose(o->fp[i]);
            o->fp[i] = 0;
        }
        o->fd[i] = -1;
    }
    pthread_mutex_unlock(&mut);
}

int
pipeclose(struct Pipe *o)
{
    closeio(o);
    return 0;
}

static void
cleanio(struct Pipe *o)
{
    closeio(o);
}

int
openwfifo(const char *name)
{
    if (genpath(buf, sizeof buf, name) != 0) {
        return -1;
    }
    return open(buf, O_WRONLY | O_NONBLOCK);
}

static int
openio(struct Pipe *o)
{
    int oflags;
    int rfd, wfd;

    pthread_mutex_lock(&mut);
    if (genpath(buf, sizeof buf, o->name) != 0) {
        pthread_mutex_unlock(&mut);
        return -1;
    }
    remove(buf);
    if (mkfifo(buf, S_IRWXU) != 0) {
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    oflags = O_RDONLY | O_NONBLOCK;
    if ((rfd = open(buf, oflags)) == -1) {
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    oflags = O_WRONLY | O_NONBLOCK;
    if ((wfd = open(buf, oflags)) == -1) {
        close(rfd);
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    if (o->dir == PipeDirTx) {
        o->fd[0] = wfd;
        o->fd[1] = rfd;
    } else {
        o->fd[0] = rfd;
        o->fd[1] = wfd;
    }
    o->fp[0] = fdopen(o->fd[0], o->dir == PipeDirRx ? "rb" : "wb");
    if (!o->fp[0]) {
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    o->fp[1] = fdopen(o->fd[1], o->dir == PipeDirTx ? "rb" : "wb");
    if (!o->fp[1]) {
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    pthread_mutex_unlock(&mut);
    return 0;
}

static int
initio(struct Pipe *o)
{
    int oflags;
    int rfd, wfd;
    
    rfd = wfd = -1;
    if (genpath(buf, sizeof buf, o->name) != 0)
        return -1;
    remove(buf);
    if (mkfifo(buf, S_IRWXU) != 0) {
        cleanio(o);
        return -1;
    }
    oflags = O_RDONLY | O_NONBLOCK;
    if ((rfd = open(buf, oflags)) == -1) {
        cleanio(o);
        return -1;
    }
    oflags = O_WRONLY | O_NONBLOCK;
    if ((wfd = open(buf, oflags)) == -1) {
        close(rfd);
        cleanio(o);
        return -1;
    }
    if (o->dir == PipeDirTx) {
        o->fd[0] = wfd;
        o->fd[1] = rfd;
    } else {
        o->fd[0] = rfd;
        o->fd[1] = wfd;
    }
    o->fp[0] = fdopen(o->fd[0], o->dir == PipeDirRx ? "rb" : "wb");
    if (!o->fp[0]) {
        cleanio(o);
        return -1;
    }
    o->fp[1] = fdopen(o->fd[1], o->dir == PipeDirTx ? "rb" : "wb");
    if (!o->fp[1]) {
        cleanio(o);
        return -1;
    }
    return 0;
}

int
initsh()
{
    if (prepsupdir() != 0) {
        fputs("failed to prepare the Application Support Directory.\n",
              stderr);
        return -1;
    }
    if (initio(&iofin) == -1) {
        cleanupsh();
        return -1;
    }
    if (initio(&iofout) == -1) {
        cleanupsh();
        return -1;
    }
    if (initio(&ioferr) == -1) {
        cleanupsh();
        return -1;
    }
    if (initfs() != 0) {
        cleanupsh();
        return -1;
    }
    procioinit(&procio, stdin, stdout, stderr);
    procio.pipe[0] = &iofin;
    procio.pipe[1] = &iofout;
    procio.pipe[2] = &ioferr;
    return 0;
}

void
cleanupsh()
{
    cleanio(&iofin);
    cleanio(&iofout);
    cleanio(&ioferr);
    cleanupfs();
}

int
putsonin(const char *s)
{
    pthread_mutex_lock(&mut);
    clearerr(iofin.fp[1]);
    if (fputs(s, iofin.fp[1])) {
        fflush(iofin.fp[1]);
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
}

int
getsfromin(char *b, int bsz)
{
    pthread_mutex_lock(&mut);
    if (fgets(b, bsz, iofin.fp[0])) {
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
}

int
putsonout(const char *s)
{
    pthread_mutex_lock(&mut);
    if (fputs(s, iofout.fp[0])) {
        fflush(iofout.fp[0]);
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
}

int
getsfromout(char *b, int bsz)
{
    pthread_mutex_lock(&mut);
    if (!iofout.fp[1]) {
        pthread_mutex_unlock(&mut);
        return -1;
    }
    clearerr(iofout.fp[1]);
    if (fgets(b, bsz, iofout.fp[1])) {
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
}

int
putsonerr(const char *s)
{
    pthread_mutex_lock(&mut);
    if (fputs(s, ioferr.fp[0])) {
        fflush(ioferr.fp[0]);
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
}

int
getsfromerr(char *b, int bsz)
{
    pthread_mutex_lock(&mut);
    clearerr(ioferr.fp[1]);
    if (fgets(b, bsz, ioferr.fp[1])) {
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
}

void
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

static void
callcmd(const struct Shcmd *cmd, int argc, char **argv)
{
    int i;
    struct Pipe redir;
    extern const char *applet_name;

    if (redirp) {
        if (convpath(redirp, buf, sizeof buf) != 0)
            return;
        procioinit(&procio, stdin, stdout, stderr);
        procio.pipe[0] = &iofin;
        procio.pipe[1] = &iofout;
        procio.pipe[2] = &ioferr;
        redir.dir = PipeDirTx;
        redir.name = 0;
        redir.fp[0] = fopen(buf, "w+b");
        if (redir.fp[0] == 0)
            return;
        redir.fd[0] = fileno(redir.fp[0]);
        redir.fd[1] = -1;
        redir.fp[1] = 0;
        procio.bk = procio.pipe[1];
        procio.pipe[1] = &redir;
    }
    clearerr(procio.pipe[0]->fp[0]);
    clearerr(procio.pipe[1]->fp[0]);
    clearerr(procio.pipe[2]->fp[0]);
    fcntl(iofin.fd[0], F_SETFL, fcntl(iofin.fd[0], F_GETFL) & ~O_NONBLOCK);
    applet_name = argv[0];
    cmd->f(argc, argv);
    for (i = 0; i < 3; ++i) {
        if (!procio.pipe[i])
            continue;
        if (procio.pipe[i]->dir == PipeDirTx && procio.pipe[i]->fp[0])
            fflush(procio.pipe[i]->fp[0]);
    }
    if (procio.bk) {
        if (procio.pipe[1]->fp[0])
            fclose(procio.pipe[1]->fp[0]);
        procio.pipe[1] = procio.bk;
        procio.bk = 0;
    }
    for (i = 0; i < 3; ++i) {
        if (!procio.pipe[i])
            continue;
        if (procio.pipe[i] != &iofin && procio.pipe[i] != &iofout && procio.pipe[i] != &ioferr)
            procio.pipe[i] = 0;
    }
    prociofreeio(&procio);
    if (!iofin.fp[0])
        openio(&iofin);
    if (!iofout.fp[0])
        openio(&iofout);
    if (!ioferr.fp[0])
        openio(&ioferr);
    fcntl(iofin.fd[0], F_SETFL, fcntl(iofin.fd[0], F_GETFL) | O_NONBLOCK);
    procioinit(&procio, stdin, stdout, stderr);
    procio.pipe[0] = &iofin;
    procio.pipe[1] = &iofout;
    procio.pipe[2] = &ioferr;
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
    extern FILE *yyin;

    yyin = iofin.fp[0];
    clearerr(yyin);
    yyparse();
    yyin = 0;

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
        if (cmd && cmd->name) {
            callcmd(cmd, argc, argv);
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
    if (redirp) {
        free(redirp);
        redirp = 0;
    }
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
    vfprintf(stdout, fmt, ap);
    fflush(stdout);
}

void
dbgput2(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    dbgput(fmt, ap);
    va_end(ap);
}

