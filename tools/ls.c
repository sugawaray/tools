#include "cmd.h"
#include "impl.h"
#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>

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
deprecated_ls_main(int argc, char **argv)
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
    if (prepsupdir() != 0)
        return -1;
    if (genpath(pathbuf, sizeof pathbuf, "/fout") != 0)
        return -1;
    fout = fopen(pathbuf, "w+");
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
