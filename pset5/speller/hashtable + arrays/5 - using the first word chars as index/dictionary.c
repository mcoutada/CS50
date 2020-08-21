#include "dictionary.h"
#include <string.h>
#include <stdio.h>
#define N 70
#define M 3
#define A 27

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

    char lw[LENGTH + 1];
    strcpy(lw, word);
    char *p;

    for (p = lw; *p; p++)
        if (*p > 64 && *p < 91)
            *p += 32;

    p = lw;

    int a = *p ? *p++ % 97 : 26;
    int b = *p ? *p++ % 97 : 26;
    int c = *p ? *p++ % 97 : 26;
    int d = *p ? *p++ % 97 : 26;
    int e = *p ? *p++ % 97 : 26;
    int y = *p ? hash(p) : 0;
    int z = 0;

    while (t[a][b][c][d][e][y][z][0])
        if (!strcmp(t[a][b][c][d][e][y][z++], lw))
            return true;

    return false;
}

bool load(const char *dictionary)
{
    FILE *dc = fopen(dictionary, "r");

    fseek(dc, 0, SEEK_END);
    long ldc = ftell(dc);
    fseek(dc, 0, SEEK_SET);
    // long ldc = 1439228;
    char fb[ldc + 1];
    fread(fb, 1, ldc, dc);
    fb[ldc] = 0;
    fclose(dc);

    char *sb = strtok(fb, "\n");
    char *sbs;
    int a, b, c, d, e, y, z;
    do
    {
        sbs = sb;
        a = *sbs ? *sbs++ % 97 : 26;
        b = *sbs ? *sbs++ % 97 : 26;
        c = *sbs ? *sbs++ % 97 : 26;
        d = *sbs ? *sbs++ % 97 : 26;
        e = *sbs ? *sbs++ % 97 : 26;
        y = *sbs ? hash(sbs) : 0;
        z = 0;
        cn++;
        while (t[a][b][c][d][e][y][z][0])
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