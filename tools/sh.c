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
    .origfd = -1,
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/fin",
    .fd = STDIN_FILENO,
    .flg    = "rb"
};
struct Iofile iofout = {
    .origfd = -1,
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/fout",
    .fd = STDOUT_FILENO,
    .flg    = "wb"
};
struct Iofile ioferr = {
    .origfd = -1,
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/ferr",
    .fd = STDERR_FILENO,
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
    if (*o->fp)
        fclose(*o->fp);
    o->fpalt = 0;
    o->fdalt = -1;
    o->fp = 0;
    pthread_mutex_unlock(&mut);
}

static void
cleanio(struct Iofile *o)
{
    pthread_mutex_lock(&mut);
    if (o->fpalt) {
        fclose(o->fpalt);
    }
    if (o->origfd != -1) {
        if (*o->fp)
            fclose(*o->fp);
        *o->fp = 0;
        if (dup2(o->origfd, o->fd) != -1)
            *o->fp = fdopen(o->fd, o->flg);
        if (o->fporig)
            fclose(o->fporig);
        else
            close(o->origfd);
    }
    o->fpalt = 0;
    o->fdalt = -1;
    o->fp = 0;
    o->origfd = -1;
    o->fporig = 0;
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
openio(struct Iofile *o, FILE **fp)
{
    int i;
    int oflags;
    int rfd, wfd;
    int tmpfd[3];

    pthread_mutex_lock(&mut);
    if (genpath(buf, sizeof buf, "/tmpfd") != 0) {
        pthread_mutex_unlock(&mut);
        return -1;
    }
    tmpfd[0] = tmpfd[1] = tmpfd[2] = -1;
    for (i = 0, tmpfd[i] = open(buf, O_WRONLY | O_CREAT, S_IRWXU);
        tmpfd[i] != -1 && tmpfd[i] < 3; ++i) {
        tmpfd[i] = open(buf, O_WRONLY | O_CREAT, S_IRWXU);
    }
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
    if (dup2(o->flg[0] == 'w' ? wfd : rfd, o->fd) == -1) {
        close(rfd);
        close(wfd);
        pthread_mutex_unlock(&mut);
        closeio(o);
        return -1;
    }
    for (i = 0; tmpfd[i] >= 0; ++i)
        if (tmpfd[i] != o->fd)
            close(tmpfd[i]);
    if (o->flg[0] == 'w') {
        close(wfd);
        o->fdalt = rfd;
    } else {
        close(rfd);
        o->fdalt = wfd;
    }
    o->fp = fp;
    *o->fp = fdopen(o->fd, o->flg);
    if (!(*o->fp)) {
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
initio(struct Iofile *o, FILE **fp)
{
    int oflags;
    int rfd, wfd;
    
    rfd = wfd = -1;
    if (genpath(buf, sizeof buf, o->name) != 0)
        return -1;
    o->fp = fp;
    o->origfd = dup(o->fd);
    if (o->origfd == -1)
        return -1;
    o->fporig = fdopen(o->origfd, o->flg);
    if (!o->fporig) {
        cleanio(o);
        return -1;
    }
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
    if (dup2(o->flg[0] == 'w' ? wfd : rfd, o->fd) == -1) {
        close(rfd);
        close(wfd);
        cleanio(o);
        return -1;
    }
    if (o->flg[0] == 'w') {
        close(wfd);
        o->fdalt = rfd;
    } else {
        close(rfd);
        o->fdalt = wfd;
    }
    *o->fp = fdopen(o->fd, o->flg);
    if (!(*o->fp)) {
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
    if (initio(&iofin, &stdin) == -1) {
        cleanupsh();
        return -1;
    }
    if (initio(&iofout, &stdout) == -1) {
        cleanupsh();
        return -1;
    }
    if (initio(&ioferr, &stderr) == -1) {
        cleanupsh();
        return -1;
    }
    if (*iofin.fp != stdin || *iofout.fp != stdout || *ioferr.fp != stderr) {
        cleanupsh();
        return -1;
    }
    if (fileno(*iofin.fp) != 0 || fileno(*iofout.fp) != 1 || fileno(*ioferr.fp) != 2) {
        cleanupsh();
        return -1;
    }
    if (initfs() != 0) {
        cleanupsh();
        return -1;
    }
    procioinit(&procio, *iofin.fp, *iofout.fp, *ioferr.fp);
    return 0;
}

void
cleanupsh()
{
    cleanio(&iofin);
    cleanio(&iofout);
    cleanio(&ioferr);
    cleanupfs();
    if (fileno(stdin) != 0 || fileno(stdout) != 1 || fileno(stderr) != 2) {
        return;
    }
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
#if 1
    pthread_mutex_lock(&mut);
    if (fgets(b, bsz, *iofin.fp)) {
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
#else
    if (fgets(b, bsz, stdin))
        return 0;
    else
        return -1;
#endif
}

int
putsonout(const char *s)
{
#if 1
    pthread_mutex_lock(&mut);
    if (fputs(s, *iofout.fp)) {
        fflush(*iofout.fp);
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
#else
    if (fputs(s, stdout)) {
        fflush(stdout);
        return 0;
    } else
        return -1;
#endif
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
#if 1
    pthread_mutex_lock(&mut);
    if (fputs(s, *ioferr.fp)) {
        fflush(*ioferr.fp);
        pthread_mutex_unlock(&mut);
        return 0;
    } else {
        pthread_mutex_unlock(&mut);
        return -1;
    }
#else
    if (fputs(s, stderr)) {
        fflush(stderr);
        return 0;
    } else
        return -1;
#endif
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
#if 1
    if (procio.fp[1][1])
        fflush(procio.fp[1][1]);
    if (procio.fp[2][1])
        fflush(procio.fp[2][1]);
    if (procio.redir[0])
        fclose(procio.redir[0]);
    if (!procio.fp[0][0]) {
        *iofin.fp = 0;
        closeio(&iofin);
        openio(&iofin, &stdin);
    } else if (!iofin.fpalt) {
        closeio(&iofin);
        openio(&iofin, &stdin);
    }
    if (!procio.fp[1][0]) {
        *iofout.fp = 0;
        closeio(&iofout);
        openio(&iofout, &stdout);
    }
    if (!procio.fp[2][0]) {
        *ioferr.fp = 0;
        closeio(&ioferr);
        openio(&ioferr, &stderr);
    }
#else
    if (!procio.fp[0][0]) {
        *iofin.fp = 0;
        closeio(&iofin);
        openio(&iofin, &stdin);
    } else if (!procio.fp[1][0]) {
        *iofout.fp = 0;
        closeio(&iofout);
        openio(&iofout, &stdout);
    } else if (!procio.fp[2][0]) {
        *ioferr.fp = 0;
        closeio(&ioferr);
        openio(&ioferr, &stderr);
    }
#endif
    fcntl(iofin.fd, F_SETFL, fcntl(iofin.fd, F_GETFL) | O_NONBLOCK);
    procioinit(&procio, stdin, stdout, stderr);
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
    if (iofout.fporig) {
        vfprintf(iofout.fporig, fmt, ap);
        fflush(iofout.fporig);
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

