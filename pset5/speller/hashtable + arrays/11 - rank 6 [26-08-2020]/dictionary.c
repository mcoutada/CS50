#include "dictionary.h"
#define N ((unsigned int)65536)
#define M ((unsigned int)3)

char *t[N][M];
unsigned int cn = 0;

unsigned int hash(const char *s)
{
    return true;
}

bool check(const char *word)
{

    const char *p = word;
    unsigned long h = 5381;

    while (*p)
    {
        // h = (*p & 32) ? 33 * h + *p : 33 * h + (*p | 32);
        // p++;
        h = 33 * h + (*p++ | 32);
    }

    unsigned int i = h > N ? h & (N - 1) : h, j = 0;
    char *q;

    while (t[i][j])
    {
        p = word;
        q = t[i][j];
        while (((*p++ | 32) ^ *q++) == 0)
        {
            if (*p + *q - 10 == 0)
            {
                if (j | 0)
                {
                    q = t[i][0];
                    t[i][0] = t[i][j];
                    t[i][j] = q;
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
    char *fb = mmap(0, ldc.st_size, PROT_READ, MAP_PRIVATE, dc, 0);

    unsigned int i, j;
    char *sb;
    unsigned long h;

    do
    {
        j = 0;
        h = 5381;
        sb = fb;
        while (*fb ^ 10)
        {
            h = 33 * h + *fb++;
        }
        i = h > N ? h & (N - 1) : h;

        while (t[i][j])
        {
            j++;
        }
        t[i][j] = sb;
        cn++;
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