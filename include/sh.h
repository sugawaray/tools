//
//  sh.h
//  tools
//
//  Created by Yutaka Sugawara on 2016/09/10.
//  Copyright © 2016 Yutaka Sugawara. All rights reserved.
//

#ifndef sh_h
#define sh_h

#include <sys/stat.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdint.h>

struct Wl;
struct Wl {
    char *p;
    
    struct Wl *next;
};

extern struct Wl *shwltail;
extern char *redirp;
void freeshwl();

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

char *ios_getcwd(char *b, size_t bsz);
int ios_mkdir(const char *path, mode_t mode);
int cd_main(int, char**);

enum {
    PipeDirRx,
    PipeDirTx
};

struct Pipe {
    int fd[2];
    FILE *fp[2];
    const char *name;
    int dir;
};

struct Pipeelem;

struct Pipeelem {
    struct Pipe o;
    struct Pipeelem *next;
};

struct Procio {
    FILE *fp[3];
    struct Pipe *pipe[3];
    struct Pipe *bk;
    struct Pipeelem *iotail;
};

FILE *procioconvfp(struct Procio *o, FILE *fp);
int procioconvfd(struct Procio *o, int fd);
int prociofpidx(struct Procio *o, FILE *fp);
int prociofdidx(struct Procio *o, int fd);
struct Pipe *procioallocio(struct Procio *o);
void prociofreeio(struct Procio *o);

extern struct Procio procio;
void procioinit(struct Procio *o, FILE *pin, FILE *pout, FILE *perr);

extern int pipeclose(struct Pipe *o);

extern int openwfifo(const char *name);
extern struct Pipe iofin;
extern struct Pipe iofout;
extern struct Pipe ioferr;

#endif /* sh_h */
