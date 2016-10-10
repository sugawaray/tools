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
extern int pwd_main(int, char**);
extern int true_main(int, char**);
extern int seq_main(int, char **);

struct Wl *shwltail;
struct Shcmd shcmdtab[] = {
    { "true", true_main },
    { "seq", seq_main },
    { "ls", ls_main },
    { "mkdir", mkdir_main },
    { "pwd", pwd_main },
    { 0, 0 }
};

struct Iofile {
    int origfd;
    FILE *fporig;
    int fdalt;
    FILE *fpalt;
    FILE **fp;
    const char *name;
    int fd;
    const char *flg;
};

static struct Iofile iofin = {
    .origfd = -1,
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/fin",
    .fd = STDIN_FILENO,
    .flg    = "rb"
};
static struct Iofile iofout = {
    .origfd = -1,
    .fdalt  = -1,
    .fpalt  = 0,
    .fp = 0,
    .name   = "/fout",
    .fd = STDOUT_FILENO,
    .flg    = "wb"
};
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
static char rootpath[512];

static void
cleanio(struct Iofile *o)
{
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
}

static int
initio(struct Iofile *o, FILE **fp)
{
    int oflags;
    int tmpfd;
    
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
    fclose(*fp);
    if (o->flg[0] == 'w')
        oflags = O_WRONLY;
    else
        oflags = O_RDONLY;
    oflags |= O_CREAT | O_TRUNC | O_NONBLOCK;
    if ((tmpfd = open(buf, oflags, S_IRWXU)) == -1) {
        cleanio(o);
        return -1;
    }
    if (dup2(tmpfd, o->fd) == -1) {
        close(tmpfd);
        cleanio(o);
        return -1;
    }
    *o->fp = fdopen(o->fd, o->flg);
    if (!(*o->fp)) {
        cleanio(o);
        return -1;
    }
    fseek(*o->fp, 0, SEEK_END);
    if (o->flg[0] == 'w')
        oflags = O_RDONLY;
    else
        oflags = O_WRONLY;
    oflags |= O_NONBLOCK;
    o->fdalt = open(buf, oflags);
    if (o->fdalt == -1) {
        cleanio(o);
        return -1;
    }
    o->fpalt = fdopen(o->fdalt, o->flg[0] == 'w' ? "rb" : "wb");
    if (!o->fpalt) {
        cleanio(o);
        return -1;
    }
    fseek(o->fpalt, 0, SEEK_END);
    return 0;
}

static int
init1()
{
    size_t l;
    if (convpath("/", buf, sizeof buf) != 0)
        return -1;
    l = strlen(buf);
    if (buf[l - 1] == '/')
        buf[l - 1] = '\0';
    if (chdir(buf) != 0)
        return -1;
    if (getcwd(rootpath, sizeof rootpath - 1) == 0)
        return -1;
    l = strlen(rootpath);
    if (rootpath[l - 1] != '/') {
        rootpath[l] = '/';
        rootpath[l + 1] = '\0';
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
    if (init1() != 0) {
        cleanupsh();
        return -1;
    }
    return 0;
}

void
cleanupsh()
{
    cleanio(&iofin);
    cleanio(&iofout);
    cleanio(&ioferr);
    if (fileno(stdin) != 0 || fileno(stdout) != 1 || fileno(stderr) != 2) {
        return;
    }
}

int
putsonin(const char *s)
{
    if (fputs(s, iofin.fpalt)) {
        fflush(iofin.fpalt);
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
    clearerr(iofout.fpalt);
    if (fgets(b, bsz, iofout.fpalt))
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

char *
ios_getcwd(char *b, size_t bsz)
{
    size_t l, l1;
    if (getcwd(b, bsz) == 0)
        return 0;
    l1 = strlen(b);
    b[l1] = '/';
    ++l1;
    l = strlen(rootpath);
    memmove(b, b + l - 1, l1 - l + 1);
    b[l1 - l + 1] = '\0';
    return b;
}
