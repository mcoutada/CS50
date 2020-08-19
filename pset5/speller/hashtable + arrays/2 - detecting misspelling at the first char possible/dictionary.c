// Implements a dictionary's functionality

#include <stdbool.h>
#include "dictionary.h"
#include <string.h>  //this is needed for strncpy
#include <ctype.h>   //this is needed for tolower
#include <stdio.h>   // this is needed for NULL, FILE datatype, fopen, fclose, fscanf
#include <strings.h> //this is needed for strcasecmp
#define N 250000
#define M 10

char table[N][M][45];

unsigned int dic_wrds_cnt = 0;

unsigned int hash(const char *s)
{

    // djb2a hash  Ref: http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    while (*s)
    {
        hash = ((hash << 5) + hash) ^ *s++; //  hash = hash * 33 + c;  // you can also try this version
        hash = hash ^ (hash >> 16);
    }
    return hash % N;
}

bool check(const char *word)
{

    char lookup_wrd[LENGTH + 1];

    strcpy(lookup_wrd, word);

    for (char *p = lookup_wrd; *p; p++)
        *p = tolower(*p);

    int i = hash(lookup_wrd), j = 0, k = 0;

    while (table[i][j][k] != '\0')
    {
        while (table[i][j][k] == (lookup_wrd[k] >= 'A' && lookup_wrd[k] <= 'Z' ?: lookup_wrd[k]))
        {
            k++;

            if (table[i][j][k] == '\0')
                return true;
        }

        j++;
    }

    return false;
}

bool load(const char *dictionary)
{
    for (int i = 0; i < M; i++)
    {
        for (int j = 0; j < M; j++)
        {
            table[i][j][0] = '\0';
        }
    }

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

        while (table[i][j][0] != '\0')
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