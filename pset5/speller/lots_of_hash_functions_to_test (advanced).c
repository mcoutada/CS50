// This file is part of the source code provided by this article which I highly recommend to read
// https://www.strchr.com/hash_functions

// (c) Peter Kankowski, 2008. http://www.strchr.com
// Hash functions benchmark

#if _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#define  _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

//#define USE_PRIME
#define USE_CHAINING
//#define STORE_32

#define _CRT_SECURE_NO_DEPRECATE
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>
#include <intrin.h>
#if _MSC_VER
#include <nmmintrin.h>
#endif


#define NUM_OF(A) (sizeof(A)/sizeof((A)[0]))
#ifndef _MSC_VER
#define _rotl(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define _rotr(x, n) (((x) << (n)) | ((x) >> (32-(n))))
#define __forceinline inline
#endif

// ==============================================
// Hash functions


// Bernstein's hash
UINT HashBernstein(const CHAR *key, SIZE_T len) {
	UINT hash = 5381;
	for(UINT i = 0; i < len; ++i)
		hash = 33 * hash + key[i];
	return hash ^ (hash >> 16);
}

// Paul Larson (http://research.microsoft.com/~PALARSON/)
UINT HashLarson(const CHAR *key, SIZE_T len) {
	UINT hash = 0;
	for(UINT i = 0; i < len; ++i)
		hash = 101 * hash + key[i];
	return hash ^ (hash >> 16);
}

// Kernighan & Ritchie, "The C programming Language", 3rd edition.
UINT HashKernighanRitchie(const CHAR *key, SIZE_T len) {
	UINT hash = 0;
	for(UINT i = 0; i < len; ++i)
		hash = 31 * hash + key[i];
	return hash;
}

// My hash function
UINT Hash17_unrolled(const CHAR *key, SIZE_T len) {
	UINT hash = 1;
	for(UINT i = 0; i < (len & -2); i += 2) {
		hash = 17 * hash + (key[i] - ' ');
		hash = 17 * hash + (key[i+1] - ' ');
	}
	if(len & 1)
		hash = 17 * hash + (key[len-1] - ' ');
	return hash ^ (hash >> 16);
}



// A hash function with multiplier 65599 (from Red Dragon book)
UINT Hash65599(const CHAR *key, SIZE_T len) {
	UINT hash = 0;
	for(UINT i = 0; i < len; ++i)
		hash = 65599 * hash + key[i];
	return hash ^ (hash >> 16);
}

// FNV hash, http://isthe.com/chongo/tech/comp/fnv/
UINT HashFNV1a(const CHAR *key, SIZE_T len) {
	UINT hash = 2166136261;
	for(UINT i = 0; i < len; ++i)
		hash = 16777619 * (hash ^ key[i]);
	return hash ^ (hash >> 16);
}

// Peter Weinberger's hash (from Red Dragon book)
UINT HashWeinberger(const CHAR *key, SIZE_T len) {
	UINT h = 0, g;
	for(UINT i = 0; i < len; ++i) {
		h = (h << 4) + key[i];
		if(( g = (h & 0xF0000000) ) != 0)
			h ^= g >> 24 ^ g;
	}
	return h ^ (h >> 16);
}

// Ramakrishna hash
UINT HashRamakrishna(const CHAR *key, SIZE_T len) {
	UINT h = 0;
	for(UINT i = 0; i < len; ++i) {
		h ^= (h << 5) + (h >> 2) + key[i];
	}
	return h;
}

// http://en.wikipedia.org/wiki/Fletcher's_checksum
UINT HashFletcher(const CHAR * key, SIZE_T len)
{
	const USHORT * data = (const USHORT *)key;
	len /= 2;
	UINT32 sum1 = 0xFFFF, sum2 = 0xFFFF;
	while (len) {
		SIZE_T tlen = len > 360 ? 360 : len;
		len -= tlen;
		do {
			sum1 += *data++;
			sum2 += sum1;
		} while (--tlen);
		sum1 = (sum1 & 0xffff) + (sum1 >> 16);
		sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	}
	/* Second reduction step to reduce sums to 16 bits */
	sum1 = (sum1 & 0xffff) + (sum1 >> 16);
	sum2 = (sum2 & 0xffff) + (sum2 >> 16);
	return sum2 << 16 | sum1;
}

// http://en.wikipedia.org/wiki/Adler-32
UINT32 HashAdler(const CHAR * data, SIZE_T len)
{
	UINT32 a = 1, b = 0;

	while(len > 0) {
		SIZE_T tlen = len > 5550 ? 5550 : len;
		len -= tlen;
		do {
			a += *data++;
			b += a;
		} while (--tlen);

		a %= 65521;
		b %= 65521;
	}
	return (b << 16) | a; 
}

// http://murmurhash.googlepages.com/MurmurHash2.cpp
UINT HashMurmur2(const CHAR * key, SIZE_T len)
{
	// 'm' and 'r' are mixing constants generated offline.
	// They're not really 'magic', they just happen to work well.

	const unsigned int m = 0x5bd1e995;
	const int r = 24;

	// Initialize the hash to a 'random' value
	UINT seed = 0x3FB0BB5F;
	unsigned int h = seed ^ (UINT)len;

	// Mix 4 bytes at a time into the hash

	const unsigned char * data = (const unsigned char *)key;

	while(len >= 4)
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

	switch(len)
	{
	case 3: h ^= data[2] << 16;
	case 2: h ^= data[1] << 8;
	case 1: h ^= data[0];
	        h *= m;
	};

	// Do a few final mixes of the hash to ensure the last few
	// bytes are well-incorporated.

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}

// MurmurHash2A, by Austin Appleby

// This is a variant of MurmurHash2 modified to use the Merkle-Damgard
// construction. Bulk speed should be identical to Murmur2, small-key speed
// will be 10%-20% slower due to the added overhead at the end of the hash.

// This variant fixes a minor issue where null keys were more likely to
// collide with each other than expected, and also makes the algorithm
// more amenable to incremental implementations. All other caveats from
// MurmurHash2 still apply.

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

UINT HashMurmur2A(const CHAR * key, SIZE_T len)
{
	const unsigned int m = 0x5bd1e995;
	const int r = 24;
	unsigned int l = (UINT)len;

	const unsigned char * data = (const unsigned char *)key;

	const UINT seed = 0x3FB0BB5F;
	unsigned int h = seed;

	while(len >= 4)
	{
		unsigned int k = *(unsigned int*)data;

		mmix(h,k);

		data += 4;
		len -= 4;
	}

	unsigned int t = 0;

	switch(len)
	{
	case 3: t ^= data[2] << 16;
	case 2: t ^= data[1] << 8;
	case 1: t ^= data[0];
	};

	mmix(h,t);
	mmix(h,l);

	h ^= h >> 13;
	h *= m;
	h ^= h >> 15;

	return h;
}


// Murmur3 hash (32-bit), http://code.google.com/p/smhasher/
UINT HashMurmur3(const CHAR* key, SIZE_T len)
{
	const UCHAR * data = (const UCHAR*)key;
	const int nblocks = len / 4;
	UINT32 h1 = 1688943522; // a random seed
	const UINT32 c1 = 0xcc9e2d51, c2 = 0x1b873593;

	// body
	const UINT32 * blocks = (const UINT32 *)(data + nblocks*4);

	for(int i = -nblocks; i; i++) {
		UINT32 k1 = blocks[i];

		k1 *= c1;
		k1 = _rotl(k1,15);
		k1 *= c2;
    
		h1 ^= k1;
		h1 = _rotl(h1,13); 
		h1 = h1*5+0xe6546b64;
	}

	// tail
	const UINT8 * tail = (const UINT8*)(data + nblocks*4);
	UINT32 k1 = 0;

	switch(len & 3)
	{
	case 3: k1 ^= tail[2] << 16;
	case 2: k1 ^= tail[1] << 8;
	case 1: k1 ^= tail[0];
			k1 *= c1; k1 = _rotl(k1,15); k1 *= c2; h1 ^= k1;
	};

	// finalization
	h1 ^= len;

	h1 ^= h1 >> 16;
	h1 *= 0x85ebca6b;
	h1 ^= h1 >> 13;
	h1 *= 0xc2b2ae35;
	h1 ^= h1 >> 16;
	return h1;
}


// xxhash, http://code.google.com/p/xxhash/
#define XXH_PRIME1   2654435761U
#define XXH_PRIME2   2246822519U
#define XXH_PRIME3   3266489917U
#define XXH_PRIME4    668265263U
#define XXH_PRIME5   0x165667b1

// This version is for very small inputs (< 16  bytes)
__forceinline unsigned int XXH_small(const void* key, int len, unsigned int seed)
{
	const unsigned char* p = (unsigned char*)key;
	const unsigned char* const bEnd = p + len;
	unsigned int idx = seed + XXH_PRIME1;
	unsigned int crc = XXH_PRIME5;
	const unsigned char* const limit = bEnd - 4;

	while (p<limit)
	{
		crc += ((*(unsigned int*)p) + idx++);
		crc += _rotl(crc, 17) * XXH_PRIME4;
		crc *= XXH_PRIME1;
		p+=4;
	}

	while (p<bEnd)
	{
		crc += ((*p) + idx++);
		crc *= XXH_PRIME1;
		p++;
	}

	crc += len;

	crc ^= crc >> 15;
	crc *= XXH_PRIME2;
	crc ^= crc >> 13;
	crc *= XXH_PRIME3;
	crc ^= crc >> 16;

	return crc;
}

UINT HashXXHfast32(const CHAR* input, SIZE_T len)
{
	UINT seed = 1688943522; // a random seed
	// Special case, for small inputs
	if (len < 16) return XXH_small(input, len, seed);

	const unsigned char* p = (const unsigned char*)input;
	const unsigned char* const bEnd = p + len;
	unsigned int v1 = seed + XXH_PRIME1;
	unsigned int v2 = v1 * XXH_PRIME2 + len;
	unsigned int v3 = v2 * XXH_PRIME3;
	unsigned int v4 = v3 * XXH_PRIME4;  
	const unsigned char* const limit = bEnd - 16;
	unsigned int crc;

	while (p<limit)
	{
		v1 = _rotl(v1, 13) + (*(unsigned int*)p); p+=4;
		v2 = _rotl(v2, 11) + (*(unsigned int*)p); p+=4;
		v3 = _rotl(v3, 17) + (*(unsigned int*)p); p+=4;
		v4 = _rotl(v4, 19) + (*(unsigned int*)p); p+=4;
	}

	p = bEnd - 16;
	v1 += _rotl(v1, 17); v2 += _rotl(v2, 19); v3 += _rotl(v3, 13); v4 += _rotl(v4, 11); 
	v1 *= XXH_PRIME1; v2 *= XXH_PRIME1; v3 *= XXH_PRIME1; v4 *= XXH_PRIME1; 
	v1 += *(unsigned int*)p; p+=4; v2 += *(unsigned int*)p; p+=4; v3 += *(unsigned int*)p; p+=4; v4 += *(unsigned int*)p;   // p+=4;
	v1 *= XXH_PRIME2; v2 *= XXH_PRIME2; v3 *= XXH_PRIME2; v4 *= XXH_PRIME2; 
	v1 += _rotl(v1, 11); v2 += _rotl(v2, 17); v3 += _rotl(v3, 19); v4 += _rotl(v4, 13); 
	v1 *= XXH_PRIME3; v2 *= XXH_PRIME3; v3 *= XXH_PRIME3; v4 *= XXH_PRIME3;

	crc = v1 + _rotl(v2, 3) + _rotl(v3, 6) + _rotl(v4, 9);
	crc ^= crc >> 11;
	crc += (XXH_PRIME4+len) * XXH_PRIME1;
	crc ^= crc >> 15;
	crc *= XXH_PRIME2;
	crc ^= crc >> 13;

	return crc;
}

UINT HashXXHstrong32(const CHAR* input, SIZE_T len)
{
	UINT seed = 1688943522; // a random seed
	// Special case, for small inputs
	if (len < 16) return XXH_small(input, len, seed);

	const unsigned char* p = (const unsigned char*)input;
	const unsigned char* const bEnd = p + len;
	unsigned int v1 = seed + XXH_PRIME1;
	unsigned int v2 = v1 * XXH_PRIME2 + len;
	unsigned int v3 = v2 * XXH_PRIME3;
	unsigned int v4 = v3 * XXH_PRIME4;  
	const unsigned char* const limit = bEnd - 16;
	unsigned int crc;

	while (p<limit)
	{
		v1 += _rotl(v1, 13); v1 *= XXH_PRIME1; v1 += (*(unsigned int*)p); p+=4;
		v2 += _rotl(v2, 11); v2 *= XXH_PRIME1; v2 += (*(unsigned int*)p); p+=4;
		v3 += _rotl(v3, 17); v3 *= XXH_PRIME1; v3 += (*(unsigned int*)p); p+=4;
		v4 += _rotl(v4, 19); v4 *= XXH_PRIME1; v4 += (*(unsigned int*)p); p+=4;
	}

	p = bEnd - 16;
	v1 += _rotl(v1, 17); v2 += _rotl(v2, 19); v3 += _rotl(v3, 13); v4 += _rotl(v4, 11); 
	v1 *= XXH_PRIME1; v2 *= XXH_PRIME1; v3 *= XXH_PRIME1; v4 *= XXH_PRIME1; 
	v1 += *(unsigned int*)p; p+=4; v2 += *(unsigned int*)p; p+=4; v3 += *(unsigned int*)p; p+=4; v4 += *(unsigned int*)p;   // p+=4;
	v1 *= XXH_PRIME2; v2 *= XXH_PRIME2; v3 *= XXH_PRIME2; v4 *= XXH_PRIME2; 
	v1 += _rotl(v1, 11); v2 += _rotl(v2, 17); v3 += _rotl(v3, 19); v4 += _rotl(v4, 13); 
	v1 *= XXH_PRIME3; v2 *= XXH_PRIME3; v3 *= XXH_PRIME3; v4 *= XXH_PRIME3;

	crc = v1 + _rotl(v2, 3) + _rotl(v3, 6) + _rotl(v4, 9);
	crc ^= crc >> 11;
	crc += (XXH_PRIME4+len) * XXH_PRIME1;
	crc ^= crc >> 15;
	crc *= XXH_PRIME2;
	crc ^= crc >> 13;

	return crc;
}




// Paul Hsieh, http://www.azillionmonkeys.com/qed/hash.html
UINT HashPaulHsieh(const CHAR* key, SIZE_T len) {
	UINT hash = (UINT)len, tmp;
	if(len == 0) return 0;

    SIZE_T rem = len & 3;
	len >>= 2;

	/* Main loop */
	for(;len > 0; len--) {
		hash  += *(const WORD*)key;
		tmp    = (*(const WORD*) (key+2) << 11) ^ hash;
		hash   = (hash << 16) ^ tmp;
		key   += 2 * sizeof (WORD);
		hash  += hash >> 11;
	}

	/* Handle end cases */
	switch(rem) {
		case 3:
			hash += *(const WORD*)key;
			hash ^= hash << 16;
			hash ^= key[sizeof (WORD)] << 18;
			hash += hash >> 11;
			break;
		case 2:
			hash += *(const WORD*)key;
			hash ^= hash << 11;
			hash += hash >> 17;
			break;
		case 1:
			hash += *key;
			hash ^= hash << 10;
			hash += hash >> 1;
	}

	/* Force "avalanching" of final 127 bits */
	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return hash;
}

// Bob Jenkins, http://www.burtleburtle.net/bob/hash/doobs.html
UINT HashOneAtTime(const CHAR* key, SIZE_T len) {
	UINT hash, i;
	for(hash=0, i=0; i<len; ++i) {
    	hash += key[i];
    	hash += (hash << 10);
    	hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

// Arash Partow, http://www.partow.net/programming/hashfunctions/
UINT HashArashPartow(const CHAR* key, SIZE_T len) {
	UINT hash = 0xAAAAAAAA;
	UINT i    = 0;

	for(i = 0; i < (len & -2); i += 2) {
		hash ^=   (hash <<  7) ^ (key[i]) ^ (hash >> 3);
        hash ^= ~((hash << 11) ^ (key[i+1]) ^ (hash >> 5));
	}
	if(len & 1) {
		hash ^= (hash <<  7) ^ (key[len - 1]) ^ (hash >> 3);
	}

	return hash;
}

// CRC-32
#define CRCPOLY 0xEDB88320
#define CRCINIT 0xFFFFFFFF

UINT g_crc_precalc[4][256];

void CRC32Init(void) {
	for(UINT i = 0; i <= 0xFF; i++) {
		UINT x = i;
		for(UINT j = 0; j < 8; j++)
			x = (x>>1) ^ (CRCPOLY & (-(INT)(x & 1)));
		g_crc_precalc[0][i] = x;
	}

	for(UINT i = 0; i <= 0xFF; i++) {
		UINT c = g_crc_precalc[0][i];
		for(UINT j = 1; j < 4; j++) {
			c = g_crc_precalc[0][c & 0xFF] ^ (c >> 8);
			g_crc_precalc[j][i] = c;
		}
	}
}

UINT CRC32(const CHAR* key, SIZE_T len) {
	UINT crc = CRCINIT;
	SIZE_T ndwords = len / sizeof(DWORD);
	for(; ndwords; ndwords--) {
		crc ^= *(DWORD*)key;
		crc =
			g_crc_precalc[3][(crc      ) & 0xFF] ^
			g_crc_precalc[2][(crc >>  8) & 0xFF] ^
			g_crc_precalc[1][(crc >> 16) & 0xFF] ^
			g_crc_precalc[0][(crc >> 24) & 0xFF];
		key += sizeof(DWORD);
	}
	if (len & sizeof(WORD)) {
		UINT c = crc ^ *(WORD*)key;
		crc = g_crc_precalc[1][(c     ) & 0xFF] ^
			  g_crc_precalc[0][(c >> 8) & 0xFF] ^ (crc >> 16);
		key += sizeof(WORD);
	}
	if (len & sizeof(BYTE))
		crc = g_crc_precalc[0][(crc ^ *key) & 0xFF] ^ (crc >> 8);
	return ~crc;
}

#if _MSC_VER
UINT CRC32Accelerated(const CHAR* key, SIZE_T len) {
	UINT crc = CRCINIT;
	SIZE_T ndwords = len / sizeof(DWORD);
	for(; ndwords; ndwords--) {
		crc = _mm_crc32_u32(crc, *(DWORD*)key);
		key += sizeof(DWORD);
	}
	if (len & sizeof(WORD)) {
		crc = _mm_crc32_u16(crc, *(WORD*)key);
		key += sizeof(WORD);
	}
	if (len & sizeof(BYTE))
		crc = _mm_crc32_u8(crc, *key);
	return crc;
}
#endif

// === Universal hash from Sedgewick's book "Algorithms in C", part 4 ===
UINT HashUniversal(const CHAR* key, SIZE_T len) {
	UINT hash = 0, a = 31415, b = 27183;
	for(UINT i = 0; i < len; ++i) {
		hash = a * hash + key[i];
		a *= b;
	}
	return hash;
}

// === lookup3 function by Bob Jenkins ===

#define mix(a,b,c) \
{ \
  a -= c;  a ^= _rotl(c, 4);  c += b; \
  b -= a;  b ^= _rotl(a, 6);  a += c; \
  c -= b;  c ^= _rotl(b, 8);  b += a; \
  a -= c;  a ^= _rotl(c,16);  c += b; \
  b -= a;  b ^= _rotl(a,19);  a += c; \
  c -= b;  c ^= _rotl(b, 4);  b += a; \
} 
#define final(a,b,c) \
{ \
  c ^= b; c -= _rotl(b,14); \
  a ^= c; a -= _rotl(c,11); \
  b ^= a; b -= _rotl(a,25); \
  c ^= b; c -= _rotl(b,16); \
  a ^= c; a -= _rotl(c,4);  \
  b ^= a; b -= _rotl(a,14); \
  c ^= b; c -= _rotl(b,24); \
} 

UINT HashLookup3( const CHAR* key, SIZE_T length) {
  UINT a,b,c;                                          /* internal state */
  union { const void *ptr; size_t i; } u;

  /* Set up the internal state */
  a = b = c = 0xdeadbeef + ((UINT32)length);

  u.ptr = key;

  if ((u.i & 0x3) == 0) {
    const UINT32 *k = (const UINT32 *)key;         /* read 32-bit chunks */

    /*------ all but last block: aligned reads and affect 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      b += k[1];
      c += k[2];
      mix(a,b,c);
      length -= 12;
      k += 3;
    }

    /*----------------------------- handle the last (probably partial) block */
    /* 
     * "k[2]&0xffffff" actually reads beyond the end of the string, but
     * then masks off the part it's not allowed to read.  Because the
     * string is aligned, the masked-off tail is in the same word as the
     * rest of the string.  Every machine with memory protection I've seen
     * does it on word boundaries, so is OK with this.  But VALGRIND will
     * still catch it and complain.  The masking trick does make the hash
     * noticably faster for short strings (like English words).
     */
#ifndef VALGRIND

    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=k[2]&0xffffff; b+=k[1]; a+=k[0]; break;
    case 10: c+=k[2]&0xffff; b+=k[1]; a+=k[0]; break;
    case 9 : c+=k[2]&0xff; b+=k[1]; a+=k[0]; break;
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=k[1]&0xffffff; a+=k[0]; break;
    case 6 : b+=k[1]&0xffff; a+=k[0]; break;
    case 5 : b+=k[1]&0xff; a+=k[0]; break;
    case 4 : a+=k[0]; break;
    case 3 : a+=k[0]&0xffffff; break;
    case 2 : a+=k[0]&0xffff; break;
    case 1 : a+=k[0]&0xff; break;
    case 0 : return c;              /* zero length strings require no mixing */
    }

#else /* make valgrind happy */

    k8 = (const BYTE *)k;
    switch(length)
    {
    case 12: c+=k[2]; b+=k[1]; a+=k[0]; break;
    case 11: c+=((UINT32)k8[10])<<16;  /* fall through */
    case 10: c+=((UINT32)k8[9])<<8;    /* fall through */
    case 9 : c+=k8[8];                   /* fall through */
    case 8 : b+=k[1]; a+=k[0]; break;
    case 7 : b+=((UINT32)k8[6])<<16;   /* fall through */
    case 6 : b+=((UINT32)k8[5])<<8;    /* fall through */
    case 5 : b+=k8[4];                   /* fall through */
    case 4 : a+=k[0]; break;
    case 3 : a+=((UINT32)k8[2])<<16;   /* fall through */
    case 2 : a+=((UINT32)k8[1])<<8;    /* fall through */
    case 1 : a+=k8[0]; break;
    case 0 : return c;
    }

#endif /* !valgrind */

  } else if ((u.i & 0x1) == 0) {
    const WORD *k = (const WORD *)key;         /* read 16-bit chunks */
    const BYTE  *k8;

    /*--------------- all but last block: aligned reads and different mixing */
    while (length > 12)
    {
      a += k[0] + (((UINT32)k[1])<<16);
      b += k[2] + (((UINT32)k[3])<<16);
      c += k[4] + (((UINT32)k[5])<<16);
      mix(a,b,c);
      length -= 12;
      k += 6;
    }

    /*----------------------------- handle the last (probably partial) block */
    k8 = (const BYTE *)k;
    switch(length)
    {
    case 12: c+=k[4]+(((UINT32)k[5])<<16);
             b+=k[2]+(((UINT32)k[3])<<16);
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 11: c+=((UINT32)k8[10])<<16;     /* fall through */
    case 10: c+=k[4];
             b+=k[2]+(((UINT32)k[3])<<16);
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 9 : c+=k8[8];                      /* fall through */
    case 8 : b+=k[2]+(((UINT32)k[3])<<16);
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 7 : b+=((UINT32)k8[6])<<16;      /* fall through */
    case 6 : b+=k[2];
             a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 5 : b+=k8[4];                      /* fall through */
    case 4 : a+=k[0]+(((UINT32)k[1])<<16);
             break;
    case 3 : a+=((UINT32)k8[2])<<16;      /* fall through */
    case 2 : a+=k[0];
             break;
    case 1 : a+=k8[0];
             break;
    case 0 : return c;                     /* zero length requires no mixing */
    }

  } else {                        /* need to read the key one byte at a time */
    const BYTE *k = (const BYTE *)key;

    /*--------------- all but the last block: affect some 32 bits of (a,b,c) */
    while (length > 12)
    {
      a += k[0];
      a += ((UINT32)k[1])<<8;
      a += ((UINT32)k[2])<<16;
      a += ((UINT32)k[3])<<24;
      b += k[4];
      b += ((UINT32)k[5])<<8;
      b += ((UINT32)k[6])<<16;
      b += ((UINT32)k[7])<<24;
      c += k[8];
      c += ((UINT32)k[9])<<8;
      c += ((UINT32)k[10])<<16;
      c += ((UINT32)k[11])<<24;
      mix(a,b,c);
      length -= 12;
      k += 12;
    }

    /*-------------------------------- last block: affect all 32 bits of (c) */
    switch(length)                   /* all the case statements fall through */
    {
    case 12: c+=((UINT32)k[11])<<24;
    case 11: c+=((UINT32)k[10])<<16;
    case 10: c+=((UINT32)k[9])<<8;
    case 9 : c+=k[8];
    case 8 : b+=((UINT32)k[7])<<24;
    case 7 : b+=((UINT32)k[6])<<16;
    case 6 : b+=((UINT32)k[5])<<8;
    case 5 : b+=k[4];
    case 4 : a+=((UINT32)k[3])<<24;
    case 3 : a+=((UINT32)k[2])<<16;
    case 2 : a+=((UINT32)k[1])<<8;
    case 1 : a+=k[0];
             break;
    case 0 : return c;
    }
  }
 
  final(a,b,c);
  return c;
} 

// Hash by David Hanson, http://www.cs.princeton.edu/software/cii/doc/atom.html
static unsigned long scatter[] = {
2078917053, 143302914, 1027100827, 1953210302, 755253631, 2002600785,
1405390230, 45248011, 1099951567, 433832350, 2018585307, 438263339,
813528929, 1703199216, 618906479, 573714703, 766270699, 275680090,
1510320440, 1583583926, 1723401032, 1965443329, 1098183682, 1636505764,
980071615, 1011597961, 643279273, 1315461275, 157584038, 1069844923,
471560540, 89017443, 1213147837, 1498661368, 2042227746, 1968401469,
1353778505, 1300134328, 2013649480, 306246424, 1733966678, 1884751139,
744509763, 400011959, 1440466707, 1363416242, 973726663, 59253759,
1639096332, 336563455, 1642837685, 1215013716, 154523136, 593537720,
704035832, 1134594751, 1605135681, 1347315106, 302572379, 1762719719,
269676381, 774132919, 1851737163, 1482824219, 125310639, 1746481261,
1303742040, 1479089144, 899131941, 1169907872, 1785335569, 485614972,
907175364, 382361684, 885626931, 200158423, 1745777927, 1859353594,
259412182, 1237390611, 48433401, 1902249868, 304920680, 202956538,
348303940, 1008956512, 1337551289, 1953439621, 208787970, 1640123668,
1568675693, 478464352, 266772940, 1272929208, 1961288571, 392083579,
871926821, 1117546963, 1871172724, 1771058762, 139971187, 1509024645,
109190086, 1047146551, 1891386329, 994817018, 1247304975, 1489680608,
706686964, 1506717157, 579587572, 755120366, 1261483377, 884508252,
958076904, 1609787317, 1893464764, 148144545, 1415743291, 2102252735,
1788268214, 836935336, 433233439, 2055041154, 2109864544, 247038362,
299641085, 834307717, 1364585325, 23330161, 457882831, 1504556512,
1532354806, 567072918, 404219416, 1276257488, 1561889936, 1651524391,
618454448, 121093252, 1010757900, 1198042020, 876213618, 124757630,
2082550272, 1834290522, 1734544947, 1828531389, 1982435068, 1002804590,
1783300476, 1623219634, 1839739926, 69050267, 1530777140, 1802120822,
316088629, 1830418225, 488944891, 1680673954, 1853748387, 946827723,
1037746818, 1238619545, 1513900641, 1441966234, 367393385, 928306929,
946006977, 985847834, 1049400181, 1956764878, 36406206, 1925613800,
2081522508, 2118956479, 1612420674, 1668583807, 1800004220, 1447372094,
523904750, 1435821048, 923108080, 216161028, 1504871315, 306401572,
2018281851, 1820959944, 2136819798, 359743094, 1354150250, 1843084537,
1306570817, 244413420, 934220434, 672987810, 1686379655, 1301613820,
1601294739, 484902984, 139978006, 503211273, 294184214, 176384212,
281341425, 228223074, 147857043, 1893762099, 1896806882, 1947861263,
1193650546, 273227984, 1236198663, 2116758626, 489389012, 593586330,
275676551, 360187215, 267062626, 265012701, 719930310, 1621212876,
2108097238, 2026501127, 1865626297, 894834024, 552005290, 1404522304,
48964196, 5816381, 1889425288, 188942202, 509027654, 36125855,
365326415, 790369079, 264348929, 513183458, 536647531, 13672163,
313561074, 1730298077, 286900147, 1549759737, 1699573055, 776289160,
2143346068, 1975249606, 1136476375, 262925046, 92778659, 1856406685,
1884137923, 53392249, 1735424165, 1602280572
};

UINT HashHanson(const CHAR* key, SIZE_T len) {
	UINT hash = 0;
	for (UINT i = 0; i < len; i++)
		hash = (hash<<1) + scatter[(unsigned char)key[i]];
	return hash;
}



static unsigned char rijndaelSBox[] = {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5,
	0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0,
	0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc,
	0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a,
	0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0,
	0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b,
	0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85,
	0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5,
	0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17,
	0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88,
	0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c,
	0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9,
	0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6,
	0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e,
	0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94,
	0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68,
	0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

UINT NovakHashUnrolled( const CHAR * s, SIZE_T L )
{
	UINT h = 1;
	BYTE * t = (BYTE *)s;
	for ( SIZE_T i = 0; i < ( L & ~1 ); i += 2 ) {
		h += ( h << 1 ) + rijndaelSBox[ t[ i ] ];
		h += ( h << 1 ) + rijndaelSBox[ t[ i + 1 ] ];
	}
	if ( L & 1 )
		h += ( h << 1 ) + rijndaelSBox[ t[ L - 1 ] ];
	return h;
}



// Hash functions by Alexander Myasnikov, http://amsoftware.narod.ru/algo.html

static const unsigned char sTable[256] =
{
  0xa3,0xd7,0x09,0x83,0xf8,0x48,0xf6,0xf4,0xb3,0x21,0x15,0x78,0x99,0xb1,0xaf,0xf9,
  0xe7,0x2d,0x4d,0x8a,0xce,0x4c,0xca,0x2e,0x52,0x95,0xd9,0x1e,0x4e,0x38,0x44,0x28,
  0x0a,0xdf,0x02,0xa0,0x17,0xf1,0x60,0x68,0x12,0xb7,0x7a,0xc3,0xe9,0xfa,0x3d,0x53,
  0x96,0x84,0x6b,0xba,0xf2,0x63,0x9a,0x19,0x7c,0xae,0xe5,0xf5,0xf7,0x16,0x6a,0xa2,
  0x39,0xb6,0x7b,0x0f,0xc1,0x93,0x81,0x1b,0xee,0xb4,0x1a,0xea,0xd0,0x91,0x2f,0xb8,
  0x55,0xb9,0xda,0x85,0x3f,0x41,0xbf,0xe0,0x5a,0x58,0x80,0x5f,0x66,0x0b,0xd8,0x90,
  0x35,0xd5,0xc0,0xa7,0x33,0x06,0x65,0x69,0x45,0x00,0x94,0x56,0x6d,0x98,0x9b,0x76,
  0x97,0xfc,0xb2,0xc2,0xb0,0xfe,0xdb,0x20,0xe1,0xeb,0xd6,0xe4,0xdd,0x47,0x4a,0x1d,
  0x42,0xed,0x9e,0x6e,0x49,0x3c,0xcd,0x43,0x27,0xd2,0x07,0xd4,0xde,0xc7,0x67,0x18,
  0x89,0xcb,0x30,0x1f,0x8d,0xc6,0x8f,0xaa,0xc8,0x74,0xdc,0xc9,0x5d,0x5c,0x31,0xa4,
  0x70,0x88,0x61,0x2c,0x9f,0x0d,0x2b,0x87,0x50,0x82,0x54,0x64,0x26,0x7d,0x03,0x40,
  0x34,0x4b,0x1c,0x73,0xd1,0xc4,0xfd,0x3b,0xcc,0xfb,0x7f,0xab,0xe6,0x3e,0x5b,0xa5,
  0xad,0x04,0x23,0x9c,0x14,0x51,0x22,0xf0,0x29,0x79,0x71,0x7e,0xff,0x8c,0x0e,0xe2,
  0x0c,0xef,0xbc,0x72,0x75,0x6f,0x37,0xa1,0xec,0xd3,0x8e,0x62,0x8b,0x86,0x10,0xe8,
  0x08,0x77,0x11,0xbe,0x92,0x4f,0x24,0xc5,0x32,0x36,0x9d,0xcf,0xf3,0xa6,0xbb,0xac,
  0x5e,0x6c,0xa9,0x13,0x57,0x25,0xb5,0xe3,0xbd,0xa8,0x3a,0x01,0x05,0x59,0x2a,0x46
};

UINT HashMaPrime2c (const CHAR *str, SIZE_T len)
{
  UINT hash = (UINT)len;
  const UINT PRIME_MULT = 1717;

  for (SIZE_T i = 0; i < len; i++, str++) {
      hash ^= sTable[( *(UCHAR*)str + i) & 255];
      hash = hash * PRIME_MULT;
  }

  return hash;
}




// SBoxHash by Bret Mulvey (http://home.comcast.net/~bretm/hash/10.html)
static const UINT sbox[] = {
    0xF53E1837, 0x5F14C86B, 0x9EE3964C, 0xFA796D53,  0x32223FC3, 0x4D82BC98, 0xA0C7FA62, 0x63E2C982,
    0x24994A5B, 0x1ECE7BEE, 0x292B38EF, 0xD5CD4E56,  0x514F4303, 0x7BE12B83, 0x7192F195, 0x82DC7300,
    0x084380B4, 0x480B55D3, 0x5F430471, 0x13F75991,  0x3F9CF22C, 0x2FE0907A, 0xFD8E1E69, 0x7B1D5DE8,
    0xD575A85C, 0xAD01C50A, 0x7EE00737, 0x3CE981E8,  0x0E447EFA, 0x23089DD6, 0xB59F149F, 0x13600EC7,
    0xE802C8E6, 0x670921E4, 0x7207EFF0, 0xE74761B0,  0x69035234, 0xBFA40F19, 0xF63651A0, 0x29E64C26,
    0x1F98CCA7, 0xD957007E, 0xE71DDC75, 0x3E729595,  0x7580B7CC, 0xD7FAF60B, 0x92484323, 0xA44113EB,
    0xE4CBDE08, 0x346827C9, 0x3CF32AFA, 0x0B29BCF1,  0x6E29F7DF, 0xB01E71CB, 0x3BFBC0D1, 0x62EDC5B8,
    0xB7DE789A, 0xA4748EC9, 0xE17A4C4F, 0x67E5BD03,  0xF3B33D1A, 0x97D8D3E9, 0x09121BC0, 0x347B2D2C,
    0x79A1913C, 0x504172DE, 0x7F1F8483, 0x13AC3CF6,  0x7A2094DB, 0xC778FA12, 0xADF7469F, 0x21786B7B,
    0x71A445D0, 0xA8896C1B, 0x656F62FB, 0x83A059B3,  0x972DFE6E, 0x4122000C, 0x97D9DA19, 0x17D5947B,
    0xB1AFFD0C, 0x6EF83B97, 0xAF7F780B, 0x4613138A,  0x7C3E73A6, 0xCF15E03D, 0x41576322, 0x672DF292,
    0xB658588D, 0x33EBEFA9, 0x938CBF06, 0x06B67381,  0x07F192C6, 0x2BDA5855, 0x348EE0E8, 0x19DBB6E3,
    0x3222184B, 0xB69D5DBA, 0x7E760B88, 0xAF4D8154,  0x007A51AD, 0x35112500, 0xC9CD2D7D, 0x4F4FB761,
    0x694772E3, 0x694C8351, 0x4A7E3AF5, 0x67D65CE1,  0x9287DE92, 0x2518DB3C, 0x8CB4EC06, 0xD154D38F,
    0xE19A26BB, 0x295EE439, 0xC50A1104, 0x2153C6A7,  0x82366656, 0x0713BC2F, 0x6462215A, 0x21D9BFCE,
    0xBA8EACE6, 0xAE2DF4C1, 0x2A8D5E80, 0x3F7E52D1,  0x29359399, 0xFEA1D19C, 0x18879313, 0x455AFA81,
    0xFADFE838, 0x62609838, 0xD1028839, 0x0736E92F,  0x3BCA22A3, 0x1485B08A, 0x2DA7900B, 0x852C156D,
    0xE8F24803, 0x00078472, 0x13F0D332, 0x2ACFD0CF,  0x5F747F5C, 0x87BB1E2F, 0xA7EFCB63, 0x23F432F0,
    0xE6CE7C5C, 0x1F954EF6, 0xB609C91B, 0x3B4571BF,  0xEED17DC0, 0xE556CDA0, 0xA7846A8D, 0xFF105F94,
    0x52B7CCDE, 0x0E33E801, 0x664455EA, 0xF2C70414,  0x73E7B486, 0x8F830661, 0x8B59E826, 0xBB8AEDCA,
    0xF3D70AB9, 0xD739F2B9, 0x4A04C34A, 0x88D0F089,  0xE02191A2, 0xD89D9C78, 0x192C2749, 0xFC43A78F,
    0x0AAC88CB, 0x9438D42D, 0x9E280F7A, 0x36063802,  0x38E8D018, 0x1C42A9CB, 0x92AAFF6C, 0xA24820C5,
    0x007F077F, 0xCE5BC543, 0x69668D58, 0x10D6FF74,  0xBE00F621, 0x21300BBE, 0x2E9E8F46, 0x5ACEA629,
    0xFA1F86C7, 0x52F206B8, 0x3EDF1A75, 0x6DA8D843,  0xCF719928, 0x73E3891F, 0xB4B95DD6, 0xB2A42D27,
    0xEDA20BBF, 0x1A58DBDF, 0xA449AD03, 0x6DDEF22B,  0x900531E6, 0x3D3BFF35, 0x5B24ABA2, 0x472B3E4C,
    0x387F2D75, 0x4D8DBA36, 0x71CB5641, 0xE3473F3F,  0xF6CD4B7F, 0xBF7D1428, 0x344B64D0, 0xC5CDFCB6,
    0xFE2E0182, 0x2C37A673, 0xDE4EB7A3, 0x63FDC933,  0x01DC4063, 0x611F3571, 0xD167BFAF, 0x4496596F,
    0x3DEE0689, 0xD8704910, 0x7052A114, 0x068C9EC5,  0x75D0E766, 0x4D54CC20, 0xB44ECDE2, 0x4ABC653E,
    0x2C550A21, 0x1A52C0DB, 0xCFED03D0, 0x119BAFE2,  0x876A6133, 0xBC232088, 0x435BA1B2, 0xAE99BBFA,
    0xBB4F08E4, 0xA62B5F49, 0x1DA4B695, 0x336B84DE,  0xDC813D31, 0x00C134FB, 0x397A98E6, 0x151F0E64,
    0xD9EB3E69, 0xD3C7DF60, 0xD2F2C336, 0x2DDD067B,  0xBD122835, 0xB0B3BD3A, 0xB0D54E46, 0x8641F1E4,
    0xA0B38F96, 0x51D39199, 0x37A6AD75, 0xDF84EE41,  0x3C034CBA, 0xACDA62FC, 0x11923B8B, 0x45EF170A,
};

UINT HashSBox (const CHAR *str, SIZE_T len) {
    UINT hash = 0;
	BYTE * t = (BYTE *)str;
    for (SIZE_T i = 0; i < len; i++) {
        hash ^= sbox[ t[i] ];
        hash *= 3;
    }
    return hash;
}

// ============================
// Hashes by Georgi 'Sanmayce'
UINT HashAlfalfa(const CHAR *key, SIZE_T wrdlen)
{
	UINT hash = 7;
	for (SIZE_T i = 0; i < (wrdlen & -2); i += 2) {
		hash = (17+9) * ((17+9) * hash + (key[i])) + (key[i+1]);
	}
	if (wrdlen & 1)
		hash = (17+9) * hash + (key[wrdlen-1]);
	return hash ^ (hash >> 16);
}

UINT HashAlfalfa_HALF(const CHAR *key, SIZE_T wrdlen)
{
	UINT hash = 12;
	UINT hashBUFFER;
	SIZE_T i;
	for(i = 0; i < (wrdlen & -4); i += 4) {
		//hash = (( ((hash<<5)-hash) + key[i] )<<5) - ( ((hash<<5)-hash) + key[i] ) + (key[i+1]);
		hashBUFFER = ((hash<<5)-hash) + key[i];
		hash = (( hashBUFFER )<<5) - ( hashBUFFER ) + (key[i+1]);
		//hash = (( ((hash<<5)-hash) + key[i+2] )<<5) - ( ((hash<<5)-hash) + key[i+2] ) + (key[i+3]);
		hashBUFFER = ((hash<<5)-hash) + key[i+2];
		hash = (( hashBUFFER )<<5) - ( hashBUFFER ) + (key[i+3]);
	}
	for(SIZE_T j = 0; j < (wrdlen & 3); j += 1) {
		hash = ((hash<<5)-hash) + key[i+j];
	}
	return hash ^ (hash >> 16);
}

UINT HashAlfalfa_DWORD(const CHAR *key, SIZE_T wrdlen)
{
	SIZE_T i, j;
	UINT hash = 7;
	for(i = 0; i < (wrdlen & -(INT_PTR)sizeof(DWORD)); i += sizeof(DWORD)) {
		DWORD x = *(DWORD*)(key + i);
		hash = (17+9) * ((17+9) * hash + ((x>>0)&0xFF) ) + ((x>>8)&0xFF);
		hash = (17+9) * ((17+9) * hash + ((x>>16)&0xFF) ) + ((x>>24)&0xFF);
	}
	for(j = 0; j < (wrdlen & (sizeof(DWORD) - 1)); j++) {
		hash = (17+9) * hash + key[i+j];
	}
	return hash ^ (hash >> 16);
}

#ifdef _WIN64
UINT HashAlfalfa_QWORD(const CHAR *key, SIZE_T wrdlen)
{
	UINT64 hashQWORD;
	UINT hashAlfalfa = 7;
	
	for(; wrdlen >= 8; wrdlen -= 8, key += 8) {
		hashQWORD=*(unsigned long long*)key;
		hashAlfalfa = (17+9) * ((17+9) * hashAlfalfa + ((hashQWORD>>0)&0xFF) ) + ((hashQWORD>>8)&0xFF);
		hashAlfalfa = (17+9) * ((17+9) * hashAlfalfa + ((hashQWORD>>16)&0xFF) ) + ((hashQWORD>>24)&0xFF);
		hashAlfalfa = (17+9) * ((17+9) * hashAlfalfa + ((hashQWORD>>32)&0xFF) ) + ((hashQWORD>>40)&0xFF);
		hashAlfalfa = (17+9) * ((17+9) * hashAlfalfa + ((hashQWORD>>48)&0xFF) ) + ((hashQWORD>>56)&0xFF);
	}
	for(; wrdlen; wrdlen--, key++)
		hashAlfalfa = (17+9) * hashAlfalfa + *key;
	return hashAlfalfa ^ (hashAlfalfa >> 16);
}
#endif


UINT HashFNV1A_unrolled(const CHAR *str, SIZE_T wrdlen)
{ 
	//const UINT PRIME = 31;
	UINT hash = 2166136261;
	const CHAR * p = str;
	/*
	// Reduce the number of multiplications by unrolling the loop
	for (SIZE_T ndwords = wrdlen / sizeof(DWORD); ndwords; --ndwords) {
	//hash = (hash ^ *(DWORD*)p) * PRIME;
	hash = ((hash ^ *(DWORD*)p)<<5) - (hash ^ *(DWORD*)p);

	p += sizeof(DWORD);
	}
	*/
	for(; wrdlen >= 4; wrdlen -= 4, p += 4) {
		hash = ((hash ^ *(DWORD*)p)<<5) - (hash ^ *(DWORD*)p);
	}

	// Process the remaining bytes
	/*
	for (SIZE_T i = 0; i < (wrdlen & (sizeof(DWORD) - 1)); i++) {
	//hash = (hash ^ *p++) * PRIME;
	hash = ((hash ^ *p)<<5) - (hash ^ *p);
	p++;
	}
	*/
	if (wrdlen & 2) {
		hash = ((hash ^ *(WORD*)p)<<5) - (hash ^ *(WORD*)p);
		p++;p++;
	}
	if (wrdlen & 1) 
		hash = ((hash ^ *p)<<5) - (hash ^ *p);

	return (hash>>16) ^ hash;
}

#ifdef _WIN64
UINT HashFNV1A_unrolled8(const CHAR *str, SIZE_T wrdlen)
{ 
	const UINT64 prime = 1099511628211;
	UINT64 hash64 = 14695981039346656037;
	
	SIZE_T wrdlen_OCTUPLET = wrdlen / sizeof(UINT64);
	
	// The goal of stage #1: to reduce number of 'imul's and mainly: the number of loops.
	
	// Stage #1:
	for (; wrdlen_OCTUPLET != 0; --wrdlen_OCTUPLET) {
		hash64 = (hash64 ^ *(UINT64 *)str) * prime; // SLOWer but with carry
		str += sizeof(UINT64);
	}
	
	// Stage #2:
	for (wrdlen_OCTUPLET = 0; wrdlen_OCTUPLET < (wrdlen & (sizeof(UINT64) - 1)); wrdlen_OCTUPLET++)
		hash64 = (hash64 ^ *str++) * prime; // SLOWer but with carry

	// 5*13 = 64+1 or 1*12+4*13 = 64 i.e. shift by 12,25,38,51
	return (UINT)((hash64>>(64-(1*12+0*13))) ^ (hash64>>(64-(1*12+1*13))) ^
	       (hash64>>(64-(1*12+2*13))) ^ (hash64>>(64-(1*12+3*13))) ^ hash64); // SLOWer but with carry
}
#endif

UINT Hash_WHIZ(const char *str, SIZE_T wrdlen)
{
	const UINT PRIME = 1607;

	UINT hash32 = 2166136261;
	const char *p = str;

	for(; wrdlen >= sizeof(DWORD); wrdlen -= sizeof(DWORD), p += sizeof(DWORD)) {
		hash32 = (hash32 ^ *(DWORD *)p) * PRIME;
	}
	if (wrdlen & sizeof(WORD)) {
		hash32 = (hash32 ^ *(WORD*)p) * PRIME;
		p += sizeof(WORD);
	}
	if (wrdlen & 1) 
		hash32 = (hash32 ^ *p) * PRIME;

	return hash32 ^ (hash32 >> 16);
}

UINT Hash_Jesteress(const char *str, SIZE_T wrdlen)
{
    const UINT PRIME = 709607;
    UINT hash32 = 2166136261;
    const char *p = str;

    for(; wrdlen >= 2*sizeof(DWORD); wrdlen -= 2*sizeof(DWORD), p += 2*sizeof(DWORD)) {
        hash32 = (hash32 ^ (_rotl(*(DWORD *)p,5) ^ *(DWORD *)(p+4))) * PRIME;        
    }
    // Cases: 0,1,2,3,4,5,6,7
    if (wrdlen & sizeof(DWORD)) {
        hash32 = (hash32 ^ *(DWORD*)p) * PRIME;
        p += sizeof(DWORD);
    }
    if (wrdlen & sizeof(WORD)) {
        hash32 = (hash32 ^ *(WORD*)p) * PRIME;
        p += sizeof(WORD);
    }
    if (wrdlen & 1) 
        hash32 = (hash32 ^ *p) * PRIME;
    
    return hash32 ^ (hash32 >> 16);
}

UINT Hash_Meiyan(const char *str, SIZE_T wrdlen)
{
    const UINT PRIME = 709607;
    UINT hash32 = 2166136261;
    const char *p = str;

    for(; wrdlen >= 2*sizeof(DWORD); wrdlen -= 2*sizeof(DWORD), p += 2*sizeof(DWORD)) {
        hash32 = (hash32 ^ (_rotl(*(DWORD *)p,5) ^ *(DWORD *)(p+4))) * PRIME;        
    }
    // Cases: 0,1,2,3,4,5,6,7
    if (wrdlen & sizeof(DWORD)) {
		hash32 = (hash32 ^ *(WORD*)p) * PRIME;
		p += sizeof(WORD);
		hash32 = (hash32 ^ *(WORD*)p) * PRIME;
		p += sizeof(WORD);
    }
    if (wrdlen & sizeof(WORD)) {
        hash32 = (hash32 ^ *(WORD*)p) * PRIME;
        p += sizeof(WORD);
    }
    if (wrdlen & 1) 
        hash32 = (hash32 ^ *p) * PRIME;
    
    return hash32 ^ (hash32 >> 16);
}

UINT Hash_Mantis(const char *str, SIZE_T wrdlen)
{
	const UINT PRIME = 709607;
	UINT hash32 = 2166136261;
	const char *p = str;
	// Cases: 0,1,2,3,4,5,6,7
	if (wrdlen & sizeof(DWORD)) {
		hash32 = (hash32 ^ *(WORD*)p) * PRIME;
		p += sizeof(WORD);
		hash32 = (hash32 ^ *(WORD*)p) * PRIME;
		p += sizeof(WORD);
		//wrdlen -= sizeof(DWORD);
	}
	if (wrdlen & sizeof(WORD)) {
		hash32 = (hash32 ^ *(WORD*)p) * PRIME;
		p += sizeof(WORD);
		//wrdlen -= sizeof(WORD);
	}
	if (wrdlen & 1) {
		hash32 = (hash32 ^ *p) * PRIME;
		p += sizeof(char);
		//wrdlen -= sizeof(char);
	}
	wrdlen -= p-str;
// The goal is to avoid the weak range [8, 8+2, 8+1] that is 8..10 in practice 1..15 i.e. 1..8+4+2+1, thus amending FNV1A_Meiyan and FNV1A_Jesteress.
// FNV1A_Jesteress: fastest strong
// FNV1A_Meiyan   : faster  stronger
// FNV1A_Mantis   : fast    strongest
	if (wrdlen) {
		for(; wrdlen > 2*sizeof(DWORD); wrdlen -= 2*sizeof(DWORD), p += 2*sizeof(DWORD)) {
			hash32 = (hash32 ^ (_rotl(*(DWORD *)p,5)^*(DWORD *)(p+4))) * PRIME;		
		}
		hash32 = (hash32 ^ *(WORD*)(p+0*sizeof(WORD))) * PRIME;
		hash32 = (hash32 ^ *(WORD*)(p+1*sizeof(WORD))) * PRIME;
		hash32 = (hash32 ^ *(WORD*)(p+2*sizeof(WORD))) * PRIME;
		hash32 = (hash32 ^ *(WORD*)(p+3*sizeof(WORD))) * PRIME;
	} // Bug Fixed!
	return hash32 ^ (hash32 >> 16);
}


// Tuned for lowercase-and-uppercase letters i.e. 26 ASCII symbols 65-90 and 97-122 decimal.
UINT Hash_Sixtinsensitive(const CHAR *str, SIZE_T wrdlen)
{ 
	UINT hash = 2166136261;
	UINT hashBUFFER_EAX, hashBUFFER_BH, hashBUFFER_BL;
	const CHAR * p = str;

	// Ox41 = 065 'A' 010 [0 0001]
	// Ox5A = 090 'Z' 010 [1 1010]
	// Ox61 = 097 'a' 011 [0 0001]
	// Ox7A = 122 'z' 011 [1 1010]

	// Reduce the number of multiplications by unrolling the loop
	for(; wrdlen >= 6; wrdlen -= 6, p += 6) {
		//hashBUFFER_AX = (*(DWORD*)(p+0)&0xFFFF);
		hashBUFFER_EAX = (*(DWORD*)(p+0)&0x1F1F1F1F);
		hashBUFFER_BL = (*(p+4)&0x1F);
		hashBUFFER_BH = (*(p+5)&0x1F);
		//6bytes-in-4bytes or 48bits-to-30bits
		// Two times next:
		//3bytes-in-2bytes or 24bits-to-15bits
		//EAX BL BH
		//[5bit][3bit][5bit][3bit][5bit][3bit][5bit][3bit]
		// 5th[0..15] 13th[0..15]
		// BL lower 3 BL higher 2bits
		// OR or XOR no difference
		hashBUFFER_EAX = hashBUFFER_EAX ^ ((hashBUFFER_BL&0x07)<<5); // BL lower 3bits of 5bits
		hashBUFFER_EAX = hashBUFFER_EAX ^ ((hashBUFFER_BL&0x18)<<(2+8)); // BL higher 2bits of 5bits
		hashBUFFER_EAX = hashBUFFER_EAX ^ ((hashBUFFER_BH&0x07)<<(5+16)); // BH lower 3bits of 5bits
		hashBUFFER_EAX = hashBUFFER_EAX ^ ((hashBUFFER_BH&0x18)<<((2+8)+16)); // BH higher 2bits of 5bits
		//hash = (hash ^ hashBUFFER_EAX)*1607; //What a mess: <<7 becomes imul but <<5 not!?
		hash = ((hash ^ hashBUFFER_EAX)<<5) - (hash ^ hashBUFFER_EAX);
		//1607:[2118599]
		// 127:[2121081]
		// 31:[2139242]
		// 17:[2150803]
		// 7:[2166336]
		// 5:[2183044]
		//8191:[2200477]
		// 3:[2205095]
		// 257:[2206188]
	}
	// Post-Variant #1:
	for(; wrdlen; wrdlen--, p++) {
		hash = ((hash ^ (*p&0x1F))<<5) - (hash ^ (*p&0x1F));
	}
	/*
	// Post-Variant #2:
	for(; wrdlen >= 2; wrdlen -= 2, p += 2) {
	hash = ((hash ^ (*(DWORD*)p&0xFFFF))<<5) - (hash ^ (*(DWORD*)p&0xFFFF));
	}
	if (wrdlen & 1) 
	hash = ((hash ^ *p)<<5) - (hash ^ *p);
	*/
	/*
	// Post-Variant #3:
	for(; wrdlen >= 4; wrdlen -= 4, p += 4) {
	hash = ((hash ^ *(DWORD*)p)<<5) - (hash ^ *(DWORD*)p);
	}
	if (wrdlen & 2) {
	hash = ((hash ^ (*(WORD*)p))<<5) - (hash ^ (*(WORD*)p));
	p++;p++;
	}
	if (wrdlen & 1) 
	hash = ((hash ^ *p)<<5) - (hash ^ *p);
	*/
	return (hash>>16) ^ hash;
}






// === Utility functions ===

long GetTxtFileSize(FILE* f) {
	long old_pos = ftell(f);

	// Seek to the end of file
	if (fseek(f, 0, SEEK_END))
		return 0;
	long size = ftell(f);
	
	// Restore saved position
	fseek(f, old_pos, SEEK_SET);
	return size;
}


UINT NextPowerOfTwo(UINT x) {
	// Henry Warren, "Hacker's Delight", ch. 3.2
	x--;
	x |= (x >> 1);
	x |= (x >> 2);
	x |= (x >> 4);
	x |= (x >> 8);
	x |= (x >> 16);
	return x + 1;
}

UINT NextLog2(UINT x) {
	// Henry Warren, "Hacker's Delight", ch. 5.3
	if(x <= 1) return x;
	x--;
	UINT n = 0;
	UINT y;
	y = x >>16; if(y) {n += 16; x = y;}
	y = x >> 8; if(y) {n +=  8; x = y;}
	y = x >> 4; if(y) {n +=  4; x = y;}
	y = x >> 2; if(y) {n +=  2; x = y;}
	y = x >> 1; if(y) return n + 2;
	return n + x;
}

void Die(const char * msg) {
	puts(msg);
	exit(1);
}




// === Txt file ====


typedef UINT (*HASH_FUNC)(const CHAR*, SIZE_T);

typedef struct {
	const CHAR * line;
	SIZE_T len;
} LINE;

struct TEXT_BLOCK {
	struct TEXT_BLOCK * next;
	CHAR * text_end;
	CHAR text[1];
};
typedef struct TEXT_BLOCK TEXT_BLOCK;

LINE * g_lines;
TEXT_BLOCK * g_text = NULL;

SIZE_T g_lines_count, g_table_size_mask, g_table_size;

SIZE_T ReallocateLines(SIZE_T allocated_lines) {
	allocated_lines *= 2;
	g_lines = (LINE *)realloc(g_lines, allocated_lines * sizeof(LINE));
	if (!g_lines)
		Die("Not enough memory");
	return allocated_lines;
}

CHAR * AllocateText(SIZE_T block_size) {
	TEXT_BLOCK * new_block = (TEXT_BLOCK *)malloc(sizeof(TEXT_BLOCK) + block_size * sizeof(CHAR));
	if (!new_block)
		Die("Not enough memory");
	new_block->next = g_text;
	new_block->text_end = new_block->text + block_size;
	g_text = new_block;
	return new_block->text;
}

void ReadTxtFile(const char * file_name, BOOL use_ip, BOOL align) {
	FILE* file = fopen(file_name, "r");
	if(!file)
		Die("Can't open the file");

	// Allocate g_text
	long file_size = GetTxtFileSize(file);
	const SIZE_T MAX_LINE_LEN = 128;
	CHAR * text = AllocateText(file_size + MAX_LINE_LEN);

	// Allocate g_lines
	SIZE_T allocated_lines = file_size / 16; // First approximation
	g_lines = (LINE *)malloc(allocated_lines * sizeof(LINE));
	if (!g_lines)
		Die("Not enough memory");

	SIZE_T i = 0;

	// Read the file line-by-line into g_text buffer
	if(use_ip) {
		CHAR ip[17];
		ULONG * ips = (ULONG *)text;
		while( fgets(ip, NUM_OF(ip), file) ) {
			if('\0' == *ip)
				continue;
			if (i >= allocated_lines)
				allocated_lines = ReallocateLines(allocated_lines);
			g_lines[i].line = (CHAR *)ips;
			g_lines[i++].len = sizeof(ULONG);
			*ips++ = inet_addr(ip);
		}
	} else {
		while( fgets(text, (UINT)(g_text->text_end - text), file) ) {
			SIZE_T len = strlen(text);
			if (text[len-1] == '\n')
				text[--len] = '\0';
			if (0 == len)
				continue;
			if (i >= allocated_lines)
				allocated_lines = ReallocateLines(allocated_lines);
			g_lines[i].line = text;
			g_lines[i++].len = len;
			text += len + 1;

			// Align the allocated lines by 8 bytes
			if (align)
				text = (CHAR*) (((INT_PTR)text + sizeof(INT64) - 1) & (-(INT_PTR)sizeof(INT64)));
			if (g_text->text_end <= text + MAX_LINE_LEN)
				text = AllocateText(file_size + MAX_LINE_LEN);
		}
	}
	fclose(file);
	g_lines_count = i;
}

// Set table size in bits.
// table_size == 0 to use the default size (which is lines_count * 2 ^ table_bits_factor)
void SetTableSize(UINT table_bits, UINT table_bits_factor) {
	if (table_bits == 0)
		table_bits = NextLog2(g_lines_count) + table_bits_factor;
	if (table_bits + sizeof(void *) > sizeof(SIZE_T) * CHAR_BIT)
		Die("table too large (decrease the number of bits or use a 64-bit compiler)");
#ifdef USE_PRIME
	                     // 1  2  4   8  16  32  64  128  256  512  1024  2048  4096  8192
	UINT closest_prime[] = {2, 2, 5, 11, 17, 37, 67, 131, 257, 521, 1031, 2053, 4099, 8209,
						    16411, 32771, 65537, 131101, 262147, 524309, 1048583, 2097169,
							4194319, 8388617, 16777259, 33554467, 67108879,	134217757,
							268435459, 536870923, 1073741827, 2147483659};
	g_table_size = closest_prime[ table_bits ];
#else
	g_table_size = (1 << table_bits);
	g_table_size_mask = g_table_size - 1;
#endif
}

void FreeTxtFile(void) {
	free(g_lines);
	
	TEXT_BLOCK * block = g_text;
	while (block) {
		TEXT_BLOCK * next = block->next;
		free(block);
		block = next;
	}
}



// =================
// Separate chaining
#ifdef USE_CHAINING

struct CHAIN;

struct CHAIN {
	const CHAR* key;
	struct CHAIN* next;
};
typedef struct CHAIN CHAIN;

CHAIN ** g_table;
CHAIN * g_chains;
UINT g_next_free_chain;

void AllocateHashTable(void) {
	g_table = (CHAIN **)malloc(g_table_size * sizeof(CHAIN *));
	if (!g_table)
		Die("Not enough memory");
	g_chains = (CHAIN*)malloc(g_lines_count * sizeof(CHAIN));
	if (!g_chains)
		Die("Not enough memory");
}

void FreeHashTable(void) {
	free(g_table);
	free(g_chains);
}

void InitHashTable(void) {
	g_next_free_chain = 0;
	memset(g_table, 0, g_table_size * sizeof(CHAIN *));
}

BOOL Lookup(const CHAR* key, SIZE_T len, HASH_FUNC hash_func, BOOL add) {
#ifdef USE_PRIME
	UINT hash = hash_func(key, len) % g_table_size;
#else
	UINT hash = hash_func(key, len) & g_table_size_mask;
#endif
	// Look up the value in the chain
	for(CHAIN* chain = g_table[hash]; chain != NULL; chain = chain->next) {
		if( memcmp(chain->key, key, len * sizeof(CHAR)) == 0 )
			return TRUE;
	}
	// If it was not found, add it
	if(add) {
		CHAIN* next = g_table[hash];
		g_table[hash] = &g_chains[g_next_free_chain++];
		g_table[hash]->next = next;
		g_table[hash]->key = key;
	}
	return FALSE;
}

UINT CountCollisions(UINT items_count) {
	UINT cells_count = 0;
	for(UINT i = 0; i < g_table_size; i++)
		if (g_table[i])
			cells_count++;
	return items_count - cells_count;
}

// The theoretical metric from "Red Dragon Book"
double CountQuality(UINT items_count) {
	UINT sum = 0;
	for (UINT i = 0; i < g_table_size; i++) {
		UINT count = 0;
		for (CHAIN* chain = g_table[i]; chain != NULL; chain = chain->next)
			count++;
		sum += count * (count + 1);
	}
	return (double)sum * g_table_size /
		((double)items_count * (items_count + 2 * g_table_size - 1));
}

void TabulateQuality( HASH_FUNC hash_func, const CHAR* name ) {
	printf("%20s: ", name);

	InitHashTable();
	for(UINT i = 0; i < g_lines_count; ++i)
		Lookup(g_lines[i].line, g_lines[i].len, hash_func, TRUE);
	double quality = CountQuality(g_lines_count);
	char mark = ' ';
	if (quality < 0.95)
		mark = '+';
	else if (quality > 1.05)
		mark = '!';
	printf(" %8.3f %c\n", quality, mark);
}

// =================
// Open Addressing
#else

typedef struct {
	const CHAR* key;
#ifdef STORE_32
	UINT hash32;
#endif
} ITEM;

ITEM * g_table;
SIZE_T g_collisions;

void AllocateHashTable() {
	g_table = (ITEM *)malloc(g_table_size * sizeof(ITEM));
	if (!g_table)
		Die("Not enough memory");
}

void FreeHashTable() {
	free(g_table);
}

void InitHashTable() {
	memset(g_table, 0, g_table_size * sizeof(ITEM));
	g_collisions = 0;
}

BOOL Lookup(const CHAR* key, SIZE_T len, HASH_FUNC hash_func, BOOL add) {
	UINT hash32 = hash_func(key, len);
#ifdef USE_PRIME
	UINT hash = hash32 % table_size;
#else
	UINT hash = hash32 & g_table_size_mask;
#endif
	// Look up the value in the table
	while(g_table[hash].key != 0) {
#ifdef STORE_32
		if( g_table[hash].hash32 == hash32 &&
			memcmp(g_table[hash].key, key, len * sizeof(CHAR)) == 0 )
			return TRUE;
#else
		if( memcmp(g_table[hash].key, key, len * sizeof(CHAR)) == 0 )
			return TRUE;
#endif
		g_collisions++;
#ifdef USE_PRIME
		hash = (hash + 1) % g_table_size;
#else
		hash = (hash + 1) & g_table_size_mask;
#endif
	}
	// If it was not found, add it
	if(add) {
		g_table[hash].key = key;
#ifdef STORE_32
		g_table[hash].hash32 = hash32;
#endif
	}
	return FALSE;
}

UINT CountCollisions(UINT items_count) {
	items_count = items_count;
	return g_collisions;
}
#endif


// ======= Benchmarks ============

#if defined(_M_IX86)
UINT64 inline GetRDTSC(void) {
   __asm {
      ; Flush the pipeline
      XOR eax, eax
      CPUID
      ; Get RDTSC counter in edx:eax
      RDTSC
   }
}
#elif defined(_M_X64)
UINT64 inline GetRDTSC(void) {
	return __rdtsc();
}
#else
UINT64 inline GetRDTSC(void) {
	return GetTickCount();
}
#endif

void BenchmarkHashFunction( HASH_FUNC hash_func, const CHAR* name ) {
	printf("%20s: ", name);
	UINT64 min_time = (UINT64)-1;
	
	// Lower number of repetitions for very large arrays
	UINT repetitions = g_lines_count > 40000 ? 10 : 40;

	for (UINT k = 0; k < repetitions; k++) {
		InitHashTable();
		UINT64 start = GetRDTSC();

		for (UINT i = 0; i < g_lines_count; ++i)
			Lookup(g_lines[i].line, g_lines[i].len, hash_func, TRUE);
		
		for (UINT i = 0; i < g_lines_count; ++i)
			Lookup(g_lines[i].line, g_lines[i].len, hash_func, FALSE);

		UINT64 time = GetRDTSC() - start;
		printf("%10I64d", (time + 500) / 1000);
		if (time < min_time)
			min_time = time;
	}
	printf("|%10I64d", (min_time + 500) / 1000);
	printf(" [%5d]\n", CountCollisions(g_lines_count));
}

void PrintTestWords(const HASH_FUNC hash_func[], const CHAR* name[], SIZE_T nfunc) {
	static const CHAR* word[] = {"too", "top", "tor", "tpp", "a000", "a001", "a002", "a003",
		"a004", "a005", "a006", "a007", "a008", "a009", "a010", "a", "aa", "aaa"
	};
	
	// Header
	printf( "%4c", ' ' );
	for (UINT j = 0; j < nfunc; j++) {
		CHAR func_name[16];
		strncpy(func_name, name[j], 9);
		func_name[9] = '\0';
		printf( "%10s", func_name );
	}
	printf( "\n" );

	for (UINT i = 0; i < NUM_OF(word); i++) {
		printf( "%4s", word[i] );
		for (UINT j = 0; j < nfunc; j++)
			printf( "%10x", hash_func[j](word[i], strlen(word[i])) );
		printf( "\n" );
	}
}


// ============================
// Tabulate collisions count

static const UINT TABULATE_STEPS = 8;

void TabulateHeader(void) {
	printf("%20c  ", ' ');
	SIZE_T table_size = g_table_size;
	for (UINT k = 0; k < TABULATE_STEPS; k++) {
		printf(" %8d", NextLog2(table_size));
		table_size /= 2;
	}
	printf("\n");
}

void TabulateCollisions( HASH_FUNC hash_func, const CHAR* name ) {
	printf("%20s: ", name);

	SIZE_T table_size = NextLog2(g_table_size);
	SIZE_T original_table_size = table_size;
	for (UINT k = 0; k < TABULATE_STEPS; k++) {
		InitHashTable();
		for(UINT i = 0; i < g_lines_count; ++i)
			Lookup(g_lines[i].line, g_lines[i].len, hash_func, TRUE);
		printf(" %8d", CountCollisions(g_lines_count));
		SetTableSize(--table_size, 0);
	}
	printf("\n");
	SetTableSize(original_table_size, 0);
}



int __cdecl main(int argc, char* argv[]) {
#if _MSC_VER
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) |
		_CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_DELAY_FREE_MEM_DF);
#endif

	CRC32Init();

	static const HASH_FUNC func[] = {HashBernstein, HashKernighanRitchie,
		Hash17_unrolled, Hash65599, HashFNV1a, HashUniversal,
		HashWeinberger, HashLarson,
		HashPaulHsieh, HashOneAtTime, HashLookup3, HashArashPartow,
		CRC32, HashRamakrishna, HashFletcher, HashMurmur2, HashHanson,
		NovakHashUnrolled, HashSBox, HashMaPrime2c,
		/* HashFNV1A_unrolled, HashAlfalfa, HashAlfalfa_HALF, HashAlfalfa_DWORD,
		Hash_Sixtinsensitive, Hash_WHIZ, Hash_Mantis, Hash_Jesteress, */ Hash_Meiyan,
		HashMurmur2A, HashMurmur3, HashXXHfast32, HashXXHstrong32,
#ifdef _WIN64
		HashAlfalfa_QWORD, HashFNV1A_unrolled8,
#endif
	};
	static const CHAR* name[] = {"Bernstein", "K&R",
		"x17 unrolled", "x65599", "FNV-1a", "Sedgewick",
		"Weinberger", "Larson",
		"Paul Hsieh", "One At Time", "lookup3", "Arash Partow",
		"CRC-32", "Ramakrishna", "Fletcher", "Murmur2", "Hanson",
		"Novak unrolled", "SBox", "MaPrime2c",
		/* "FNV1A_unrolled", "Alfalfa", "Alfalfa_HALF", "Alfalfa_DWORD",
		 "Hash_Sixtinsensitive", "Whiz", "Mantis", "Jesteress", */ "Meiyan",
		 "Murmur2A", "Murmur3", "XXHfast32", "XXHstrong32",
#ifdef _WIN64
		"Alfalfa_QWORD", "FNV1A_unrolled8",
#endif
	};
	assert(NUM_OF(name) == NUM_OF(func));


#ifdef _DEBUG
	// Check value from the guide by Ross N. Williams (http://www.ross.net/crc/download/crc_v3.txt)
	assert( CRC32("123456789", 9) == 0xcbf43926 );
	// And other values:
	assert( CRC32("1234567890", 10) == 0x261daee5 );
	assert( CRC32("1234567890A", 11) == 0x39f95d2 );

	// Check value from http://www.pdl.cmu.edu/mailinglists/ips/mail/msg05931.html
	CHAR crc_test_vector[32];
	memset(crc_test_vector, 0xFF, sizeof(crc_test_vector));
	UINT crc = CRC32Accelerated(crc_test_vector, sizeof(crc_test_vector));
	assert( _byteswap_ulong(~crc) == 0x43aba862 );
#endif

	BOOL is_ip = FALSE, is_collisions = FALSE, is_quality = FALSE, is_aligned = FALSE;
	const char * file_name = NULL;
	UINT table_bits = 0;

	for (int i = 1; i < argc; i++) {
		if (argv[i][0] != '-' && argv[i][0] != '/') {
			if (file_name != NULL)
				Die("More than one file name on command line");
			file_name = argv[i];
			continue;
		}
		if (strcmp(argv[i] + 1, "ip") == 0)
			is_ip = TRUE;
		else if (strcmp(argv[i] + 1, "c") == 0)
			is_collisions = TRUE;
		else if (strcmp(argv[i] + 1, "q") == 0)
			is_quality = TRUE;
		else if (strcmp(argv[i] + 1, "a") == 0)
			is_aligned = TRUE;
		else if ('s' == argv[i][1])
			table_bits = atoi(argv[i] + 2);
		else
			Die("Invalid command-line switch");
	}
	
	if (NULL == file_name)
		Die("Usage: hash TEXT-FILE [/sBITS] [/ip] [/c]\n"
			"  /s   The number of bits, e.g. /s16 for a 65536-element table. \n"
			"       If not given, a default value is used\n"
			"  /ip  Read IPv4 addresses\n"
			"  /a   Align strings in memory\n"
			"  /c   Tabulate collisions\n"
			"  /q   Hash function quality\n");

	ReadTxtFile(file_name, is_ip, is_aligned);
	SetTableSize(table_bits, is_collisions ? 4 : 1);
	printf("%d lines read\n"  "%d elements in the table (%d bits)\n",
		g_lines_count, g_table_size, NextLog2(g_table_size));
	AllocateHashTable();

	if (is_collisions) {
		TabulateHeader();
		for (UINT i = 0; i < NUM_OF(func); i++)
			TabulateCollisions(func[i], name[i]);
	} else if (is_quality) {
		for (UINT i = 0; i < NUM_OF(func); i++)
			TabulateQuality(func[i], name[i]);
	} else {
		SetThreadAffinityMask(GetCurrentThread(), 1);
		// Run the benchmarks
		//PrintTestWords(func + 6, name + 6, NUM_OF(func) - 6);
		for (UINT i = 0; i < NUM_OF(func); i++)
			BenchmarkHashFunction(func[i], name[i]);

#if _MSC_VER
		// Test CRC32 in hardware if available
		int a[4];
		__cpuid(a, 1);
		if (a[2] & (1 << 20))
			BenchmarkHashFunction(CRC32Accelerated, "iSCSI CRC");
#endif
	}

	FreeTxtFile();
	FreeHashTable();
	return 0;
}
