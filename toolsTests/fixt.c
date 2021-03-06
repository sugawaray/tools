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
    size_t l;
    char *tp;
    st = 0;

    if (strlen(name) > 16)
        return -1;
    tp = getenv("TMPDIR");
    if (!tp)
        return -1;
    if (strlen(tp) + 32 < bufsz) {
        strcpy(pathbuf, tp);
        l = strlen(pathbuf);
        if (pathbuf[l - 1] != '/')
            strcat(pathbuf, "/");
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
