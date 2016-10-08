//
//  time1.h
//  tools
//
//  Created by Yutaka Sugawara on 2016/10/08.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#ifndef time1_h
#define time1_h

#include <sys/time.h>

double difftimesec(const struct timespec *t1, const struct timespec *t0);

#endif /* time1_h */
