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
ios_lstat(const char *path, struct stat *b)
{
    if (convpath(path, pathbuf, sizeof pathbuf) == 0)
        return lstat(pathbuf, b);
    else
        return -1;
}

int
ios_stat(const char *path, struct stat *b)
{
    if (convpath(path, pathbuf, sizeof pathbuf) == 0)
        return stat(pathbuf, b);
    else
        return -1;
}

int
initbusybox()
{
    struct Platform pf;
    
    pf.opendir = ios_opendir;
    pf.lstat = ios_lstat;
    pf.stat = ios_stat;
    setplatform(&pf);
    return 0;
}
