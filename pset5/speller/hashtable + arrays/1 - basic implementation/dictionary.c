// Implements a dictionary's functionality

#include <stdbool.h>
#include "dictionary.h"
#include <string.h>  //this is needed for strncpy
#include <ctype.h>   //this is needed for tolower
#include <stdio.h>   // this is needed for NULL, FILE datatype, fopen, fclose, fscanf
#include <strings.h> //this is needed for strcasecmp
#define N 143091
#define M 10

char table[N][M][45]; // as this is declared at file scpe, all array elements are initialized to 0

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

    strcpy(lookup_wrd, word);

    for (char *p = lookup_wrd; *p; p++)
        if (*p >= 'A' && *p <= 'Z')
            *p = tolower(*p);

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

        while (table[i][j][0] != 0)
        {
            j++;
        }
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