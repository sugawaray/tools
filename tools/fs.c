//
//  fs.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "impl.h"
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <errno.h>

static char buf[512];
static char rootpath[512];

int
genpath(char *buf, int bsz, const char *suffix)
{
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

int
prepsupdir()
{
    int r;
    r = genpath(buf, sizeof buf, "");
    if (r != 0) {
        fputs("genpath failed\n", stderr);
        return -1;
    }
    errno = 0;
    r = mkdir(buf, 0700);
    if (r != 0 && errno != EEXIST)
        return -1;
    else
        return 0;
}

static inline int
pcompend(char v)
{
    return v == '/' || v == '\0';
}

static inline int
isparent(const char *p)
{
    return p[0] == '.' && p[1] == '.' && pcompend(p[2]);
}

static inline int
iscurrent(const char *p)
{
    return p[0] == '.' && pcompend(p[1]);
}

static
void
cleanup(char *b)
{
    char *p, *q;
    if (b[0] == '\0')
        return;
    q = b;
    *q = *b;
    p = b + 1;
    while (*p) {
        if (*q == '/' && isparent(p)) {
            if (q != b) {
                do {
                    --q;
                } while (q != b && *q != '/');
            }
            if (p[2] == '/')
                ++p;
        } else if (*q == '/' && iscurrent(p)) {
            if (p[1] == '/')
                ++p;
        } else if (*p != '/' || *q != '/') {
            ++q;
            *q = *p;
        }
        ++p;
    }
    ++q;
    *q = '\0';
}

static inline int
isdots(const char *p)
{
    return strcmp(p, ".") == 0 || strcmp(p, "..") == 0;
}

int
convpath1(int abs, const char *path, char *buf, size_t bsz)
{
    size_t l, l1;
    if (abs) {
        CFURLRef u = getdocdir();
        if (!CFURLGetFileSystemRepresentation(u, false,
                                              (UInt8 *)buf,
                                              bsz))
            return -1;
        l = strlen(buf);
        if (l == 0)
            return -2;
        if (buf[l - 1] != '/') {
            buf[l] = '/';
            buf[l + 1] = '\0';
            ++l;
        }
        if (strcmp(path, "") == 0 || isdots(path)) {
            if (l + 1 >= bsz)
                return -1;
            else
                return 0;
        }
    } else {
        if (getcwd(buf, bsz - 1) == 0)
            return -1;
        l = strlen(buf);
        if (buf[l - 1] != '/') {
            buf[l] = '/';
            buf[l + 1] = '\0';
            ++l;
        }
        if (l >= bsz)
            return -1;
    }
    l1 = strlen(path);
    if (l + l1 >= bsz)
        return -1;
    strcat(buf, path);
    if (abs)
        l = strlen(rootpath);
    cleanup(buf + l - 1);
    return 0;
}

int
convpath(const char *path, char *buf, size_t bsz)
{
    return convpath1(rootpath[0] == '\0', path, buf, bsz);
}

int
initfs()
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

void
cleanupfs()
{
    memset(rootpath, 0, sizeof rootpath);
}

char *
ios_getcwd(char *b, size_t bsz)
{
    size_t l, l1;
    if (getcwd(b, bsz) == 0)
        return 0;
    l = strlen(rootpath);
    l1 = strlen(b);
    if (l1 < l - 1)
        return 0;
    if (l1 == l - 1) {
        b[l1] = '/';
        ++l1;
    }
    memmove(b, b + l - 1, l1 - l + 1);
    b[l1 - l + 1] = '\0';
    return b;
}

int
cd_main(int argc, char **argv)
{
    size_t l;
    const char *p;
    if (argc > 1) {
        l = strlen(argv[1]);
        if (l < 1)
            return -1;
        if (convpath1(argv[1][0] == '/', argv[1], buf, sizeof buf) != 0)
            return 1;
        else
            p = buf;
    } else
        p = rootpath;
    if (chdir(p) != 0)
        return -1;
    else
        return 0;
}

