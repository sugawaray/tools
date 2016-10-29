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
    { 0, 0 }
};

struct Iofile iofin = {
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/fin",
    .fd = -1,
    .flg    = "rb"
};
struct Iofile iofout = {
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/fout",
    .fd = -1,
    .flg    = "wb"
};
struct Iofile ioferr = {
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/ferr",
    .fd = -1,
    .flg    = "wb"
};
static char buf[512];
struct Procio procio;
static pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

static void
closeio(struct Iofile *o)
{
    pthread_mutex_lock(&mut);
    if (o->fpalt)
        fclose(o->fpalt);
    if (o->fp)
        fclose(o->fp);
    o->fpalt = 0;
    o->fdalt = -1;
    o->fp = 0;
    pthread_mutex_unlock(&mut);
}

static void
cleanio(struct Iofile *o)
{
    pthread_mutex_lock(&mut);
    if (o->fpalt)
        fclose(o->fpalt);
    if (o->fp)
        fclose(o->fp);
    o->fpalt = 0;
    o->fdalt = -1;
    o->fp = 0;
    pthread_mutex_unlock(&mut);
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
openio(struct Iofile *o)
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
    if (o->flg[0] == 'w') {
        o->fd = wfd;
        o->fdalt = rfd;
    } else {
        o->fd = rfd;
        o->fdalt = wfd;
    }
    o->fp = fdopen(o->fd, o->flg);
    if (!o->fp) {
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    o->fpalt = fdopen(o->fdalt, o->flg[0] == 'w' ? "rb" : "wb");
    if (!o->fpalt) {
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    pthread_mutex_unlock(&mut);
    return 0;
}

static int
initio(struct Iofile *o)
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
    if (o->flg[0] == 'w') {
        o->fd = wfd;
        o->fdalt = rfd;
    } else {
        o->fd = rfd;
        o->fdalt = wfd;
    }
    o->fp = fdopen(o->fd, o->flg);
    if (!o->fp) {
        cleanio(o);
        return -1;
    }
    o->fpalt = fdopen(o->fdalt, o->flg[0] == 'w' ? "rb" : "wb");
    if (!o->fpalt) {
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
    procio.fp[0][1] = iofin.fp;
    procio.fp[1][1] = iofout.fp;
    procio.fp[2][1] = ioferr.fp;
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
    clearerr(iofin.fpalt);
    if (fputs(s, iofin.fpalt)) {
        fflush(iofin.fpalt);
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
    if (fgets(b, bsz, iofin.fp)) {
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
    if (fputs(s, iofout.fp)) {
        fflush(iofout.fp);
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
    clearerr(iofout.fpalt);
    if (fgets(b, bsz, iofout.fpalt)) {
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
    if (fputs(s, ioferr.fp)) {
        fflush(ioferr.fp);
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
    clearerr(ioferr.fpalt);
    if (fgets(b, bsz, ioferr.fpalt)) {
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
    extern const char *applet_name;
    if (redirp) {
        if (convpath(redirp, buf, sizeof buf) != 0)
            return;
        procioinit(&procio, stdin, stdout, stderr);
        procio.fp[0][1] = iofin.fp;
        procio.fp[1][1] = iofout.fp;
        procio.fp[2][1] = ioferr.fp;
        procio.redir[0] = fopen(buf, "w+b");
        if (procio.redir[0] == 0)
            return;
        procio.fp[1][1] = procio.redir[0];
    }
    clearerr(procio.fp[0][1]);
    clearerr(procio.fp[1][1]);
    clearerr(procio.fp[2][1]);
    fcntl(iofin.fd, F_SETFL, fcntl(iofin.fd, F_GETFL) & ~O_NONBLOCK);
    applet_name = argv[0];
    cmd->f(argc, argv);
    if (procio.fp[1][1])
        fflush(procio.fp[1][1]);
    if (procio.fp[2][1])
        fflush(procio.fp[2][1]);
    if (procio.redir[0])
        fclose(procio.redir[0]);
    if (!procio.fp[0][1]) {
        iofin.fp = 0;
        closeio(&iofin);
        openio(&iofin);
    } else if (!iofin.fpalt) {
        closeio(&iofin);
        openio(&iofin);
    }
    if (!procio.fp[1][1]) {
        iofout.fp = 0;
        closeio(&iofout);
        openio(&iofout);
    } else if (!iofout.fpalt) {
        closeio(&iofout);
        openio(&iofout);
    }
    if (!procio.fp[2][1]) {
        ioferr.fp = 0;
        closeio(&ioferr);
        openio(&ioferr);
    }
    fcntl(iofin.fd, F_SETFL, fcntl(iofin.fd, F_GETFL) | O_NONBLOCK);
    procioinit(&procio, stdin, stdout, stderr);
    procio.fp[0][1] = iofin.fp;
    procio.fp[1][1] = iofout.fp;
    procio.fp[2][1] = ioferr.fp;
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
    
    yyin = iofin.fp;
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

