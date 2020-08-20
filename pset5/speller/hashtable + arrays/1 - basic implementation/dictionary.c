// Implements a dictionary's functionality

#include "dictionary.h"
#include <string.h> // this is needed for strcpy strcmp
#include <stdio.h>  // this is needed for NULL FILE datatype, fopen, fclose, fscanf
#define N 85000
#define M 3

char table[N][M][45]; // as this is declared at global scope, all array elements are initialized to 0

unsigned int dic_wrds_cnt = 0;

unsigned int hash(const char *s)
{
    unsigned long hash = 0;
    while (*s)
    {
        hash = 101 * hash + *s++;
        hash = hash ^ (hash >> 16);
    }
    return hash % N;
}

bool check(const char *word)
{

    char lookup_wrd[LENGTH + 1];
    char *p;
    strcpy(lookup_wrd, word);

    for (p = lookup_wrd; *p; p++)
        if (*p > 64 && *p < 91)
            *p = *p + 32;

    int i = hash(lookup_wrd), j = 0;

    while (table[i][j][0] != 0)
    {
        if (strcmp(table[i][j++], lookup_wrd) == 0)
        {
            return true;
        }
    }

    return false;
}

bool load(const char *dictionary)
{

    FILE *dic = fopen(dictionary, "r");
    if (dic == NULL)
    {
        return false;
    }

    char buffer[LENGTH + 1];
    int i, j;

    while (fscanf(dic, "%s", buffer) != EOF)
    {
        i = hash(buffer);
        j = 0;

        while (table[i][j][0])
        {
            j++;
        };

        strcpy(table[i][j], buffer);
        dic_wrds_cnt++;
    }
    fclose(dic);

    return true;
}

// Returns number of words in dictionary if loaded else 0 if not yet loaded
unsigned int size(void)
{
    return dic_wrds_cnt;
}

bool unload(void)
{
    return true;
}