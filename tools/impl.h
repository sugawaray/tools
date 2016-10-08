//
//  impl.h
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/11.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#ifndef impl_h
#define impl_h

#include <path.h>
#include <CoreFoundation/CoreFoundation.h>

CFURLRef getdocdir();
CFURLRef getlibdir();

int prepsupdir();
int genpath(char *buf, int bsz, const char *suffix);

#endif /* impl_h */
