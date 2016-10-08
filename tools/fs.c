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
convpath(const char *path, char *buf, size_t bsz)
{
    size_t l, l1;
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
    l1 = strlen(path);
    if (l + l1 >= bsz)
        return -1;
    strcat(buf, path);
    cleanup(buf + l - 1);
    return 0;
}
