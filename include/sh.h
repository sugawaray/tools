//
//  sh.h
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/10.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#ifndef sh_h
#define sh_h

#include <stdarg.h>

struct Wl;
struct Wl {
    char *p;
    
    struct Wl *next;
};

extern struct Wl *shwltail;

int initsh();
void cleanupsh();
int putsonin(const char *s);
int getsfromin(char *b, int bsz);
int putsonout(const char *s);
int getsfromout(char *b, int bsz);
int putsonerr(const char *s);
int getsfromerr(char *b, int bsz);
void procin();
int yyparse();

struct Shcmd {
    const char *name;
    int (*f)(int, char **);
};
extern struct Shcmd shcmdtab[];
const struct Shcmd *findcmd(const char *name);
int genargv(struct Wl *first, struct Wl *end, char ***vp, int *cp);
void dbgput(const char *fmt, va_list);
void dbgput2(const char *fmt, ...);

#endif /* sh_h */
