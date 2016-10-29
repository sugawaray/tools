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

FILE *convfp(FILE *(*fps)[2], FILE *fp);
int convfd(FILE *(*fps)[2], int fd);

struct Procio {
    FILE *fp[3][2];
    FILE *redir[1];
};
extern struct Procio procio;
void procioinit(struct Procio *o, FILE *pin, FILE *pout, FILE *perr);

struct Iofile {
    int fdalt;
    FILE *fpalt;
    FILE *fp;
    const char *name;
    int fd;
    const char *flg;
};

#if 0
struct Pipe {
    int fd[2];
    FILE *fp[2];
    const char *name;
    int dir;
};

struct Procio {
    FILE *fp[3];
    struct Pipe *pipe[3];
};

extern struct Pipe stdiop[3];
/*
stdin, stdout, stderr ... dummy. don't use it.
0, 1, 2 ... dummy. don't use it.
*/
#endif

extern int openwfifo(const char *name);
extern struct Iofile iofin;
extern struct Iofile iofout;
extern struct Iofile ioferr;

#endif /* sh_h */
