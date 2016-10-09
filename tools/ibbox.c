//
//  ibbox.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <path.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdio.h>

struct Platform {
    DIR *(*opendir)(const char *);
    int (*lstat)(const char *, struct stat *);
    int (*stat)(const char *, struct stat *);
    int (*chmod)(const char *, mode_t);
    int (*mkdir)(const char *, mode_t);
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

#undef CATS
#define CATS(s1, s2) s1##s2

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

int
initbusybox()
{
    struct Platform pf;
    
    pf.opendir = ios_opendir;
    pf.lstat = ios_lstat;
    pf.stat = ios_stat;
    pf.chmod = ios_chmod;
    pf.mkdir = ios_mkdir;
    setplatform(&pf);
    return 0;
}
