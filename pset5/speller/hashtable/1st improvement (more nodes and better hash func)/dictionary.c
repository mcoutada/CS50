// Implements a dictionary's functionality

#include <stdbool.h>
#include "dictionary.h"
#include <string.h>  //this is needed for strlen, strncpy
#include <ctype.h>   //this is needed for tolower
#include <stdio.h>   // this is needed for NULL, FILE datatype, fopen, fclose, fscanf
#include <strings.h> //this is needed for strcasecmp
#include <stdlib.h>  //this is needed for malloc

// Represents a node in a hash table
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// Number of buckets in hash table
// One bucket per character in alphabet
const unsigned long long N = 1000000;

// Hash table
node *table[N];

//cnt is unsigned (positive) because it's a counter and it's what CS50 wants us to return in size()
unsigned int dic_wrds_cnt = 0;

// Returns true if word is in dictionary else false
bool check(const char *word)
{
    int len = strlen(word);
    char lookup_wrd[len + 1];

    // lower case word
    for (int i = 0; i < len; i++)
    {
        lookup_wrd[i] = tolower(word[i]);
    }

    //words need to be nul-terminated for strcasecmp to work, also the hash changes

    lookup_wrd[len] = '\0';

    node *node_ptr = table[hash(lookup_wrd)];

    while (node_ptr != NULL)
    {
        // strcmp(wrd1, wrd2) == 0 means wrd1 = wrd2, < 0 means wrd1 comes before wrd2, > 0 after
        if (strcmp(node_ptr->word, lookup_wrd) == 0)
        {
            return true;
        }
        node_ptr = node_ptr->next;
    }
    return false;
}

unsigned int djb2(const char *word)
{
    unsigned long hash = 5381;
    int c = *word;
    // c = tolower(c); // not needed we are already processing lowercase words
    while (*word != 0)
    {
        hash = ((hash << 5) + hash) + c;
        c = *word++;
        // c = tolower(c); // not needed we are already processing lowercase words
    }

    return hash % N;
}

// Ref: https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/
unsigned int hashValue(const char *word)
{
    unsigned int hash = 0;
    for (int i = 0, n = strlen(word); i < n; i++)
    {
        hash = (hash << 2) ^ word[i];
    }
    return hash % N;
}

// Hashes word to a number
unsigned int hash(const char *word)
{
    return hashValue(word); // change for either hashValue or djb2 to test
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
        // create temp node
        node *temp = malloc(sizeof(node));

        // strncpy(wrd1, wrd2, n) copies wrd1 into wrd2 up to n bytes, sizeof(wrd2)>sizeof(wrd1) it fills the rest with /0
        // as we are declaring buffer as buffer[LENGTH + 1], it will always have at least one /0
        strncpy(temp->word, buffer, sizeof(buffer));

        // all temp node values need to be initialized or valgrind will complain if we use them uninitialized
        // further reading here: https://cs50.stackexchange.com/questions/18563/conditional-jump-or-move-depends-on-uninitialised-values
        temp->next = NULL;

        // obtain word index
        int idx = hash(buffer);

        // if table[idx] is NULL (pointing nowhere), set it to point to the temp node
        if (table[idx] == NULL)
        {
            table[idx] = temp;
        }
        // else append the temp node to the start of the linked list in that table node
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

// Returns number of words in dictionary if loaded else 0 if not yet loaded
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