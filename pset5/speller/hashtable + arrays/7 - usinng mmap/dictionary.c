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

    char lw[LENGTH + 1];
    strcpy(lw, word);

    for (char *p = lw; *p; p++)
        if (*p > 64 && *p < 91)
            *p += 32;

    int i = hash(lw), j = 0;

    while (t[i][j][0])
        if (!strcmp(t[i][j++], lw))
            return true;

    return false;
}

bool load(const char *dictionary)
{
#include <sys/mman.h> // this is needed for mmap
#include <fcntl.h>    // this is needed for O_RDONLY
#include <sys/stat.h> // this is needed for the stat datatype
                      // #include <sys/types.h> // this was theorically needd but it works without it

    struct stat ldc;
    int status;

    stat(dictionary, &ldc);
    int dc = open(dictionary, O_RDONLY);
    // long ldc = 1439228;

    char *fb = mmap(0, ldc.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, dc, 0);
    char *sb = strtok(fb, "\n");
    int i, j;

    do
    {
        i = hash(sb);
        j = 0;
        cn++;
        while (t[i][j][0])
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