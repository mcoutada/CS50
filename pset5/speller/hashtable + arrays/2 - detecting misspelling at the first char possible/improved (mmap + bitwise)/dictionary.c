#include "dictionary.h"
#include <string.h>
#include <stdio.h>
#define N 70001
#define M 3

char t[N][M][LENGTH];
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

    int i = hash(lw), j = 0;

    while (*t[i][j])
    {
        p = lw;
        q = t[i][j++];

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
    int i, j;

    do
    {
        i = hash(sb);
        j = 0;
        cn++;
        while (*t[i][j])
            j++;

        strcpy(t[i][j], sb);
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