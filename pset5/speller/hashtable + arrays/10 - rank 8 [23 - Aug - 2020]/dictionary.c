#include "dictionary.h"
#include <string.h>
#define N ((unsigned int)65536)
#define M ((unsigned int)3)

static char *t[N][M];
unsigned int cn = 0;

#include <stdint.h>

unsigned int hash(const char *s)
{
    return true;
}

bool check(const char *word)
{
    char lw[LENGTH];
    strcpy(lw, word);
    char *p = lw, *q;

    unsigned long h = 5381;

    while (*p)
    {
        // *p |= 32;
        *p = !(*p & 32) ? *p | 32 : *p;
        h = 33 * h + *p++;
    }

    unsigned int i = h > N ? h % N : h, j = 0;

    while (t[i][j])
    {
        p = lw;
        q = t[i][j];

        while (!(*p++ ^ *q++))
        {
            if (*p + *q == 10)
            {
                if (j | 0)
                {
                    p = t[i][0];
                    t[i][0] = t[i][j];
                    t[i][j] = p;
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
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>

    struct stat ldc;
    stat(dictionary, &ldc);

    unsigned int dc = open(dictionary, O_RDONLY);
    char *fb = mmap(0, ldc.st_size, PROT_WRITE, MAP_PRIVATE, dc, 0);

    unsigned int i, j;
    char *sb;
    unsigned long h;

    while (*fb)
    {
        cn++;
        j = 0;
        h = 5381;
        sb = fb;
        while (*sb ^ 10)
        {
            h = 33 * h + *sb++;
        }
        i = h > N ? h % N : h;

        while (t[i][j])
        {
            j++;
        }
        t[i][j] = fb;
        fb = ++sb;
    }
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