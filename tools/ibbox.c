//
//  ibbox.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <path.h>
#include <sh.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdio.h>

struct Platform {
    DIR *(*opendir)(const char *);
    int (*lstat)(const char *, struct stat *);
    int (*stat)(const char *, struct stat *);
    int (*chmod)(const char *, mode_t);
    int (*mkdir)(const char *, mode_t);
    char *(*getcwd)(char*, size_t bsz);
    int (*creat)(const char *, mode_t);
    FILE *(*fopen)(const char *, const char *);
    int (*open)(const char *, int oflags, ...);
    int (*fflush)(FILE *);
    ssize_t (*writev)(int, const struct iovec *, int);
    int (*printf)(const char *, ...);
    int (*puts)(const char *);
    int (*close)(int);
    ssize_t (*read)(int, void *, size_t);
    ssize_t (*write)(int, const void *, size_t);
    int (*fputs)(const char *, FILE *);
    int (*fputc)(int, FILE *);
    char *(*fgets)(char *, int, FILE *);
    int (*putchar)(int);
    int (*access)(const char *, int);
    int (*fprintf)(FILE *, const char *, ...);
    int (*rmdir)(const char *);
    int (*unlink)(const char *);
    int (*dup2)(int, int);
    int (*fclose)(FILE *);
};
extern void setplatform(const struct Platform *newval);

static char pathbuf[1024];

DIR *
ios_opendir(const char *path)
{
    if (convpath(path, pathbuf, sizeof pathbuf) == 0)
        return opendir(pathbuf);
    else
        return 0;
}

int
ios_rmdir(const char *path)
{
    if (convpath(path, pathbuf, sizeof pathbuf) == 0)
        return rmdir(pathbuf);
    else
        return -1;
}

#undef CATS
#define CATS(s1, s2) s1##s2

#undef DEFA1
#define DEFA1(name, rtype, errval)  \
rtype \
CATS(ios_, name)(const char *path) \
{ \
    if (convpath(path, pathbuf, sizeof pathbuf) == 0) \
        return name(pathbuf); \
    else \
        return errval; \
} \
/**/
DEFA1(unlink, int, -1)

#undef DEFA2
#define DEFA2(name, rtype, a2type, errval)   \
rtype \
CATS(ios_, name)(const char *path, a2type a2) \
{ \
    if (convpath(path, pathbuf, sizeof pathbuf) == 0) \
        return name(pathbuf, a2); \
    else \
        return errval; \
} \
/**/

DEFA2(lstat, int, struct stat *, -1)
DEFA2(stat, int, struct stat *, -1)
DEFA2(chmod, int, mode_t, -1)
DEFA2(mkdir, int, mode_t, -1)
DEFA2(creat, int, mode_t, -1)
DEFA2(fopen, FILE *, const char *, 0)
DEFA2(access, int, int, -1)

int
ios_open(const char *path, int oflags, ...)
{
    int has3rd;
    int r;
    va_list ap;
    
    has3rd = oflags & O_CREAT;
    if (convpath(path, pathbuf, sizeof pathbuf) == 0) {
        if (has3rd) {
            va_start(ap, oflags);
            r = open(pathbuf, oflags, va_arg(ap, int));
            va_end(ap);
            return r;
        } else
            return open(pathbuf, oflags);
    } else
        return -1;
}

int
ios_fflush(FILE *fp)
{
    return fflush(convfp(procio.fp, fp));
}

ssize_t
ios_writev(int fd, const struct iovec *iov, int iovcnt)
{
    return writev(convfd(procio.fp, fd), iov, iovcnt);
}

int
ios_printf(const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = vfprintf(convfp(procio.fp, stdout), fmt, ap);
    va_end(ap);
    return r;
}

int
ios_fprintf(FILE *f, const char *fmt, ...)
{
    int r;
    va_list ap;
    va_start(ap, fmt);
    r = vfprintf(convfp(procio.fp, f), fmt, ap);
    va_end(ap);
    return r;
}

int
ios_puts(const char *s)
{
    FILE *f = convfp(procio.fp, stdout);
    if (fputs(s, f) != EOF)
        return fputc('\n', f);
    else
        return EOF;
}

int
ios_close(int fd)
{
    int i, r;
    int cfd, credir;
    cfd = convfd(procio.fp, fd);
    for (i = 0; i < 3; ++i)
        if (procio.fp[i][1] && fileno(procio.fp[i][1]) == cfd)
            break;
    credir = -1;
    if (i < 3) {
        if (procio.redir[0])
            credir = fileno(procio.redir[0]);
        r = close(cfd);
        if (r == 0) {
            fclose(procio.fp[i][0]);
            fclose(procio.fp[i][1]);
            if (cfd == credir) {
                fclose(procio.redir[0]);
                procio.redir[0] = 0;
            }
            procio.fp[i][0] = procio.fp[i][1] = 0;
            if (i == 0)
                stdin = 0;
            else if (i == 1)
                stdout = 0;
            else if (i == 2)
                stderr = 0;
        }
        return r;
    } else
        return close(cfd);
}

int
ios_fclose(FILE *fp)
{
    int i;
    int r;
    int credir;
    for (i = 0; i < 3; ++i)
        if (procio.fp[i][0] == fp)
            break;
    if (i < 3) {
        if (procio.redir[0])
            credir = fileno(procio.redir[0]);
        else
            credir = -1;
        r = fclose(fp);
        if (r == 0) {
            fclose(procio.fp[i][1]);
            if (credir >= 0) {
                fclose(procio.redir[0]);
                procio.redir[0] = 0;
            }
            procio.fp[i][0] = procio.fp[i][1] = 0;
            if (i == 0)
                stdin = 0;
            else if (i == 1)
                stdout = 0;
            else if (i == 2)
                stderr = 0;
        }
        return r;
    } else
        return fclose(fp);
}

ssize_t
ios_read(int fd, void *b, size_t nb)
{
    return read(convfd(procio.fp, fd), b, nb);
}

ssize_t
ios_write(int fd, const void *b, size_t nb)
{
    return write(convfd(procio.fp, fd), b, nb);
}

int
ios_fputs(const char *s, FILE *fp)
{
    return fputs(s, convfp(procio.fp, fp));
}

int
ios_fputc(int c, FILE *fp)
{
    return fputc(c, convfp(procio.fp, fp));
}

char *
ios_fgets(char *s, int n, FILE *fp)
{
    return fgets(s, n, convfp(procio.fp, fp));
}

int
ios_putchar(int c)
{
    return fputc(c, convfp(procio.fp, stdout));
}

int
ios_dup2(int f1, int f2)
{
    int i, j;
    int r;
    int cf1, cf2, credir;
    FILE *fp;
    cf1 = f1;
    cf2 = f2;
    for (i = 0; i < 3; ++i)
        if (procio.fp[i][0] && fileno(procio.fp[i][0]) == f1) {
            cf1 = f1;
            break;
        }
    for (j = 0; j < 3; ++j)
        if (procio.fp[j][0] && fileno(procio.fp[j][0]) == f2) {
            cf2 = f2;
            break;
        }
    if (i < 3 && j < 3 && i == j)
        return f2;
    credir = -1;
    if (procio.redir[0])
        credir = fileno(procio.redir[0]);
    if (j == 0) {
        fclose(iofin.fpalt);
        iofin.fdalt = -1;
        iofin.fpalt = 0;
    }
    r = dup2(cf1, cf2);
    if (r == -1)
        return -1;
    if (j < 3) {
        fp = fdopen(cf2, j == 0 ? "rb" : "wb");
        procio.fp[j][0] = procio.fp[j][1] = fp;
        if (credir >= 0 && credir == cf2)
            procio.redir[0] = fp;
        if (j == 0) {
            stdin = fp;
        } else if (j == 1)
            stdout = fp;
        else if (j == 2)
            stderr = fp;
    }
    return r;
}

int
initbusybox()
{
    struct Platform pf;
    
    pf.opendir = ios_opendir;
    pf.lstat = ios_lstat;
    pf.stat = ios_stat;
    pf.chmod = ios_chmod;
    pf.mkdir = ios_mkdir;
    pf.getcwd = ios_getcwd;
	pf.creat = ios_creat;
	pf.fopen = ios_fopen;
	pf.open = ios_open;
	pf.fflush = ios_fflush;
	pf.writev = ios_writev;
	pf.printf = ios_printf;
	pf.puts = ios_puts;
	pf.close = ios_close;
	pf.read = ios_read;
	pf.write = ios_write;
	pf.fputs = ios_fputs;
	pf.fputc = ios_fputc;
	pf.fgets = ios_fgets;
	pf.putchar = ios_putchar;
    pf.access = ios_access;
    pf.fprintf = ios_fprintf;
    pf.rmdir = ios_rmdir;
    pf.unlink = ios_unlink;
    pf.dup2 = ios_dup2;
    pf.fclose = ios_fclose;
    setplatform(&pf);
    return 0;
}
