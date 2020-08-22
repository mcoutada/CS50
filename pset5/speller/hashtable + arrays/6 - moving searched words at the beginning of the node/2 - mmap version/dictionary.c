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
    {
        if (!strcmp(t[i][j], lw))
        {
            if (j != 0)
            {
                strcpy(t[i][j], t[i][0]);
                strcpy(t[i][0], lw);
            }
            return true;
        }
        j++;
    }

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