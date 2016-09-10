//
//  fixt.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/09.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
mktestdir(const char *name, char *pathbuf, size_t bufsz)
{
    int st;
    char *tp;
    st = 0;

    if (strlen(name) > 16)
        return -1;
    tp = getenv("TMPDIR");
    if (!tp)
        return -1;
    if (strlen(tp) + 32 < bufsz) {
        strcpy(pathbuf, tp);
        strcat(pathbuf, name);
        strcat(pathbuf, "XXXXX");
        tp = mkdtemp(pathbuf);
        if (tp)
            return 0;
        else
            return -1;
    } else
        return -1;
}

int
replacein(const char *path, FILE **f)
{
    int r;
    *f = fopen(path, "w+b");
    if (!*f) {
        return -1;
    }
    r = fclose(stdin);
    if (r != 0) {
        fclose(*f);
    }
    stdin = *f;
    return 0;
}