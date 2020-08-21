// Implements a dictionary's functionality

#include <stdbool.h>
#include "dictionary.h"
#include <string.h>  //this is needed for strlen, strncpy
#include <ctype.h>   //this is needed for tolower
#include <stdio.h>   // this is needed for NULL, FILE datatype, fopen, fclose, fscanf
#include <strings.h> //this is needed for strcasecmp
#include <stdlib.h>  //this is needed for malloc

typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

/* english has 230k words approx, it's better to have a 6 digits hash to diminish the size of a node's linked list
   from what I read hash functions improve if you use the biggest prime number possible, for 6 digits is 999983
*/
#define N 85000

node *table[N];

unsigned int dic_wrds_cnt = 0;

unsigned int hash(const char *s)
{

    // djb2a hash  Ref: http://www.cse.yorku.ca/~oz/hash.html
    unsigned long hash = 5381;
    while (*s)
    {
        hash = (((hash << 5) + hash) ^ *s++); //  hash = hash * 33 + c;  // you can also try this version
    }
    return (hash ^ (hash >> 16)) % N;
}

bool check(const char *word)
{
    char lookup_wrd[LENGTH + 1];

    strcpy(lookup_wrd, word);

    for (char *p = lookup_wrd; *p; p++)
        *p = tolower(*p);

    node *node_ptr = table[hash(lookup_wrd)];

    while (node_ptr != NULL)
    {
        if (strcmp(node_ptr->word, lookup_wrd) == 0)
        {
            return true;
        }
        node_ptr = node_ptr->next;
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

    while (fscanf(dic, "%s", buffer) != EOF)
    {
        node *temp = malloc(sizeof(node));

        strncpy(temp->word, buffer, sizeof(buffer));

        int idx = hash(buffer);

        if (table[idx] == NULL)
        {
            temp->next = NULL;
            table[idx] = temp;
        }
        else
        {
            temp->next = table[idx];
            table[idx] = temp;
        }
        dic_wrds_cnt++;
    }
    fclose(dic);
    return true;
}

unsigned int size(void)
{
    return dic_wrds_cnt;
}

bool unload(void)
{
    for (int i = 0; i < N; i++)
    {
        node *node_ptr = table[i];

        while (node_ptr != NULL)
        {
            node *temp = node_ptr;
            node_ptr = node_ptr->next;
            free(temp);
        }
    }

    return true;
}