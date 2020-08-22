#include "dictionary.h"
#include <string.h>
#include <stdio.h>
#define N 5
#define M 2
#define A 123

char t[A][A][A][A][A][N][M][LENGTH];
unsigned int cn = 0;

unsigned int hash(const char *s)
{
    unsigned long h = 5381;
    while (*s)
        h = 33 * h + *s++;

    return h % N;
}

bool check(const char *word)
{
    char lw[LENGTH];
    strcpy(lw, word);

    char *p = lw, *q;

    while (*p)
        *p++ |= 32;

    p = lw;

    int a = *p ? *p++ : 0;
    int b = *p ? *p++ : 0;
    int c = *p ? *p++ : 0;
    int d = *p ? *p++ : 0;
    int e = *p ? *p++ : 0;
    int y = *p ? hash(p) : 0;
    int z = 0;

    while (*t[a][b][c][d][e][y][z])
    {
        p = lw;
        q = t[a][b][c][d][e][y][z++];

        while (!(*p++ ^ *q++))
            if (*q == 0 && *p == 0)
                return true;
    }

    return false;
}

bool load(const char *dictionary)
{
#include <sys/mman.h> // this is needed for mmap
#include <fcntl.h>    // this is needed for O_RDONLY
#include <sys/stat.h> // this is needed for the stat datatype
                      // #include <sys/types.h> // this was theorically needd but it works without it

    struct stat ldc;
    stat(dictionary, &ldc);
    // long ldc = 1439228;

    int dc = open(dictionary, O_RDONLY);
    char *fb = mmap(0, ldc.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, dc, 0);
    char *sb = strtok(fb, "\n");
    char *sbs;
    int a, b, c, d, e, y, z;

    do
    {
        sbs = sb;
        a = *sbs ? *sbs++ : 0;
        b = *sbs ? *sbs++ : 0;
        c = *sbs ? *sbs++ : 0;
        d = *sbs ? *sbs++ : 0;
        e = *sbs ? *sbs++ : 0;
        y = *sbs ? hash(sbs) : 0;
        z = 0;
        cn++;
        while (*t[a][b][c][d][e][y][z])
            z++;

        strcpy(t[a][b][c][d][e][y][z], sb);
    } while ((sb = strtok(NULL, "\n")));
    return true;
}

unsigned int size(void)
{
    return cn;
}

bool unload(void)
{
    return true;
}