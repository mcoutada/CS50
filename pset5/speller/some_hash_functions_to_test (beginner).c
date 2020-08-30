// Great articles to read:
// https://softwareengineering.stackexchange.com/questions/49550/which-hashing-algorithm-is-best-for-uniqueness-and-speed
// https://www.strchr.com/hash_functions?allcomments=1

// I am far from being an expert in this matter and I am only leaving this file for testing and play around.

// How can we test a function? just copy the one you like in dictionary.c and call it as below:

unsigned int hash(const char *s)
{
    unsigned long hash = djb2(s);
    return hash % N; //N is the size of you hash table
}

//If you want to save yourself a lot of testing time, I got the best results with the simplest ones, like djb2 or Larson, the one below is the one I like the most.

unsigned long djb2(const char *s)
{
    unsigned long hash = 5381;
    while (*s)
    {
        hash = hash * 33 + c;
    }
    // this line below make your hash a little slower but you will have less collisions
    // I didn't needed it, but confirmed it improves this and any hash function, below an explanation
    // hash = hash ^ (hash >> 16);
    return hash;
}

// Citated from: https://www.strchr.com/hash_functions?allcomments=1
// XORing high and low part
// For table size less than 2^16, we can improve the quality of hash function by XORing high and low words, so that more letters will be taken into account:
// return hash ^ (hash >> 16);

// a simple way to test how many collisions you are having, is to use the counter from the SIZE function,
// just count every time a node is created and it's not the first one for that hash
// you will see the amount of collisions as the "WORDS IN DICTIONARY: "

//**********************************************************************************//
// Some hash functions to test:                                                     //
//**********************************************************************************//

// Ref: https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/
unsigned long hashValue(const char *s)
{
    unsigned long hash = 0;
    while (*s)
    {
        hash = (hash << 2) ^ *s++;
    }
    // hash = hash ^ (hash >> 16);
    return hash;
}

// Ref: http://www.cse.yorku.ca/~oz/hash.html
unsigned long djb2(const char *s) // older djb2 version yet the most popular
{
    unsigned long hash = 5381;
    while (*s)
    {
        hash = ((hash << 5) + hash) + *s++;
        //  hash = hash * 33 + c;  // you can also try this version
    }
    // hash = hash ^ (hash >> 16);
    return hash;
}

// Ref: http://www.cse.yorku.ca/~oz/hash.html
unsigned long djb2a(const char *s) // newer version(^ is preferred over +)
{
    unsigned long hash = 5381;
    while (*s)
    {
        hash = ((hash << 5) + hash) ^ *s++;
        //  hash = hash * 33 ^ c;               // you can also try this version
    }
    // hash = hash ^ (hash >> 16);
    return hash;
}

// Ref: http://www.cse.yorku.ca/~oz/hash.html
unsigned long sdbm(const char *s)
{
    unsigned long hash = 0;
    while (*s)
        hash = *s++ + (hash << 6) + (hash << 16) - hash;

    // hash = hash ^ (hash >> 16);
    return hash;
}

// Ref: http://isthe.com/chongo/tech/comp/fnv/
unsigned long fnv1(const char *s)
{
    unsigned long hash = 2166136261U;
    while (*s)
    {
        hash = hash ^ *s++;     // xor next byte into the bottom of the hash
        hash = hash * 16777619; // Multiply by prime number found to work well
    }
    // hash = hash ^ (hash >> 16);
    return hash;
}

// Ref: http://isthe.com/chongo/tech/comp/fnv/
unsigned long fnv1a(const char *s)
{
    unsigned long hash = 2166136261;
    while (*s)
    {
        hash = hash * 16777619 * (hash ^ *s++);
    }
    // hash = hash ^ (hash >> 16);
    return hash;
}

// Ref: http://research.microsoft.com/~PALARSON/
unsigned long larson(const char *s)
{
    unsigned long hash = 0;
    while (*s)
    {
        hash = 101 * hash + *s++;
    }
    // hash = hash ^ (hash >> 16);
    return hash;
}

//Ref: https://www.strchr.com/hash_functions#comment_776
#include <stdint.h>
#define _rotl_KAZE(x, n) (((x) << (n)) | ((x) >> (32 - (n))))
#define _PADr_KAZE(x, n) (((x) << (n)) >> (n))
#define ROLInBits 27 // 5 in r.1; Caramba: it should be ROR by 5 not ROL, from the very beginning the idea was to mix two bytes by shifting/masking the first 5 'noisy' bits (ASCII 0-31 symbols).
// CAUTION: Add 8 more bytes to the buffer being hashed, usually malloc(...+8) - to prevent out of boundary reads!
unsigned long FNV1A_Hash_Yorikke_v3(const char *p)
{
    uint32_t wrdlen = strlen(p);
    const uint32_t PRIME = 591798841;
    uint32_t hash32 = 2166136261;
    uint64_t PADDEDby8;
    for (; wrdlen > 2 * sizeof(uint32_t); wrdlen -= 2 * sizeof(uint32_t), p += 2 * sizeof(uint32_t))
    {
        hash32 = (_rotl_KAZE(hash32, ROLInBits) ^ (*(uint32_t *)(p + 0))) * PRIME;
        hash32 = (_rotl_KAZE(hash32, ROLInBits) ^ (*(uint32_t *)(p + 4))) * PRIME;
    }
    // Here 'wrdlen' is 1..8
    PADDEDby8 = _PADr_KAZE(*(uint64_t *)(p + 0), (8 - wrdlen) << 3); // when (8-8) the QWORD remains intact
    hash32 = (_rotl_KAZE(hash32, ROLInBits) ^ *(uint32_t *)((char *)&PADDEDby8 + 0)) * PRIME;
    hash32 = (_rotl_KAZE(hash32, ROLInBits) ^ *(uint32_t *)((char *)&PADDEDby8 + 4)) * PRIME;
    hash32 = hash32 ^ (hash32 >> 16);
    return hash32;
}
// Last touch: 2019-Oct-03, Kaze

// http://murmurhash.googlepages.com/MurmurHash2.cpp
unsigned int hash(const char *key)
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    unsigned int len = strlen(key);
    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value
    unsigned int seed = 0x3FB0BB5F;
    unsigned int h = seed ^ (unsigned int)len;

    // Mix 4 bytes at a time into the hash

    const unsigned char *data = (const unsigned char *)key;

    while (len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array

    switch (len)
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
