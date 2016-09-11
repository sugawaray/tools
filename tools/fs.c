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
