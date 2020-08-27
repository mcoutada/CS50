#include "dictionary.h"
#include <string.h>
#include <stdio.h>
#define A 123
#define N ((unsigned int)4)
#define M ((unsigned int)1)

char *t[A][A][A][A][A][N][M];
unsigned int cn = 0;

unsigned int hash(const char *s)
{
    unsigned long h = 5381;
    while (*s)
        h = 33 * h + (*s++ | 32);

    return h > N ? h & (N - 1) : h;
}

bool check(const char *word)
{
    const char *p = word;
    char *q;

    int a = *p ? (*p++ | 32) : 0;
    int b = *p ? (*p++ | 32) : 0;
    int c = *p ? (*p++ | 32) : 0;
    int d = *p ? (*p++ | 32) : 0;
    int e = *p ? (*p++ | 32) : 0;
    int i = *p ? hash(p) : 0;
    int j = 0;

    while (t[a][b][c][d][e][i][j])
    {
        p = word;
        q = t[a][b][c][d][e][i][j];

        while (((*p++ | 32) ^ *q++) == 0)
        {
            if (*p + *q - 10 == 0)
            {
                if (j | 0)
                {
                    q = t[a][b][c][d][e][i][j - 1];
                    t[a][b][c][d][e][i][j - 1] = t[a][b][c][d][e][i][j];
                    t[a][b][c][d][e][i][j] = q;
                }
                return true;
            }
        }
        j++;
    }
    return false;
}

bool load(const char *dictionary)
{
#include <sys/mman.h> // this is needed for mmap
#include <fcntl.h>    // this is needed for O_RDONLY
#include <sys/stat.h> // this is needed for the stat datatype

    struct stat ldc;
    stat(dictionary, &ldc);
    // long ldc = 1439228;

    int dc = open(dictionary, O_RDONLY);
    char *fb = mmap(0, ldc.st_size, PROT_READ, MAP_PRIVATE, dc, 0);
    int a, b, c, d, e, i, j;
    unsigned long h;

    char *sb;

    do
    {
        sb = fb;
        a = (*fb ^ 10) ? *fb++ : 0;
        b = (*fb ^ 10) ? *fb++ : 0;
        c = (*fb ^ 10) ? *fb++ : 0;
        d = (*fb ^ 10) ? *fb++ : 0;
        e = (*fb ^ 10) ? *fb++ : 0;
        j = 0;
        cn++;

        if (*fb ^ 10)
        {
            h = 5381;
            while (*fb ^ 10)
            {
                h = 33 * h + *fb++;
            }

            i = h > N ? h & (N - 1) : h;
        }
        else
        {
            i = 0;
        }

        while (t[a][b][c][d][e][i][j])
        {
            j++;
        }

        t[a][b][c][d][e][i][j] = sb;
    } while (*++fb);

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