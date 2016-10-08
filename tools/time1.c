//
//  time1.c
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#include "time1.h"
#include <stdio.h>

double
difftimesec(const struct timespec *t1, const struct timespec *t0)
{
    double r;
    r = t1->tv_sec - t0->tv_sec;
    r += (t1->tv_nsec - t0->tv_nsec) / 1000000000.0;
    return r;
}
