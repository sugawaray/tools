#include "cmd.h"
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <dirent.h>

extern CFURLRef getdocdir();
extern CFURLRef getlibdir();

static char pathbuf[1024];
static FILE *fout;

static
DIR *
mopendir(const char *path)
{
    CFURLRef u = getdocdir();
    if (!CFURLGetFileSystemRepresentation(u, false,
                                          (UInt8 *)pathbuf,
                                          sizeof pathbuf))
        return NULL;
    else
        return opendir(pathbuf);
}

static
struct dirent *
mreaddir(DIR *d)
{
    return readdir(d);
}

static
int
mclosedir(DIR *d)
{
    return closedir(d);
}

int
ls_main(int argc, char **argv)
{
    int r;
    DIR *d;
    struct dirent *e;
    long off;
    
    d = mopendir("/");
    if (!d) {
        return 1;
    }
    fseek(fout, 0, SEEK_END);
    off = ftell(fout);
    while ((e = mreaddir(d))) {
        r = fprintf(fout, "%s\n", e->d_name);
    }
    fflush(fout);
    fseek(fout, off, SEEK_SET);
    mclosedir(d);
    return 0;
}

int
cmdinit()
{
    size_t l;
    size_t l2;
    CFURLRef u = getlibdir();
    if (!CFURLGetFileSystemRepresentation(u, false,
                                          (UInt8 *)pathbuf,
                                          sizeof pathbuf))
        return -1;
    l = strlen(pathbuf);
    if (l == sizeof pathbuf - 1)
        return -1;
    l2 = strlen("/fout");
    if (sizeof pathbuf - l <= l2)
        return -1;
    strcat(pathbuf, "/fout");
    fout = fopen(pathbuf, "r+");
    if (fout == NULL)
        return -1;
    else
        return 0;
}

char *
mgets(char *b, int n)
{
    return fgets(b, n, fout);
}