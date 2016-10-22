//
//  io_c.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/22.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include <stdio.h>

FILE *
convfp(FILE *(*fps)[2], FILE *fp)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (fps[i][0] == fp)
            break;
    }
    if (i < 3)
        return fps[i][1];
    else
        return fp;
}

int
convfd(FILE *(*fps)[2], int fd)
{
    int i;
    for (i = 0; i < 3; ++i) {
        if (fileno(fps[i][0]) == fd)
            break;
    }
    if (i < 3)
        return fileno(fps[i][1]);
    else
        return fd;
}
