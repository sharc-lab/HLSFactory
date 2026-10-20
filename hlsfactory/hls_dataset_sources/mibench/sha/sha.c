/* NIST Secure Hash Algorithm */
/* heavily modified by Uwe Hollerbach uh@alumni.caltech edu */
/* from Peter C. Gutmann's implementation as found in */
/* Applied Cryptography by Bruce Schneier */
/* NIST's proposed modification to SHA of 7/11/94 may be */
/* activated by defining USE_MODIFIED_SHA */
/*
 * MiBench security/sha.
 *
 * HLS adaptation: 32-bit types, explicit big-endian byte loads instead of
 * byte_reverse(), no FILE streaming; sha_init/sha_update/sha_final are
 * folded into one function over a message buffer.
 */
#include "sha.h"

/* SHA f()-functions */
#define f1(x, y, z) (((x) & (y)) | (~(x) & (z)))
#define f2(x, y, z) ((x) ^ (y) ^ (z))
#define f3(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define f4(x, y, z) ((x) ^ (y) ^ (z))

/* SHA constants */
#define CONST1 0x5a827999u
#define CONST2 0x6ed9eba1u
#define CONST3 0x8f1bbcdcu
#define CONST4 0xca62c1d6u

/* 32-bit rotate */
#define ROT32(x, n) (((x) << (n)) | ((x) >> (32 - (n))))

#define FUNC(n, i)                                          \
    temp = ROT32(A, 5) + f##n(B, C, D) + E + W[i] + CONST##n; \
    E = D;                                                  \
    D = C;                                                  \
    C = ROT32(B, 30);                                       \
    B = A;                                                  \
    A = temp

/* do SHA transformation on one 64 byte block */
static void sha_transform(uint32_t digest[5], const uint8_t block[SHA_BLOCKSIZE])
{
    int i;
    uint32_t temp, A, B, C, D, E, W[80];

    for (i = 0; i < 16; ++i) {
        W[i] = ((uint32_t)block[4 * i] << 24) | ((uint32_t)block[4 * i + 1] << 16) |
               ((uint32_t)block[4 * i + 2] << 8) | (uint32_t)block[4 * i + 3];
    }
    for (i = 16; i < 80; ++i) {
        W[i] = W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16];
#ifdef USE_MODIFIED_SHA
        W[i] = ROT32(W[i], 1);
#endif
    }
    A = digest[0];
    B = digest[1];
    C = digest[2];
    D = digest[3];
    E = digest[4];
    for (i = 0; i < 20; ++i) {
        FUNC(1, i);
    }
    for (i = 20; i < 40; ++i) {
        FUNC(2, i);
    }
    for (i = 40; i < 60; ++i) {
        FUNC(3, i);
    }
    for (i = 60; i < 80; ++i) {
        FUNC(4, i);
    }
    digest[0] += A;
    digest[1] += B;
    digest[2] += C;
    digest[3] += D;
    digest[4] += E;
}

void sha_digest(const uint8_t msg[SHA_MAX_LEN], unsigned int len, uint32_t digest[5])
{
    uint32_t d[5];
    uint8_t block[SHA_BLOCKSIZE];
    unsigned int nfull = len / SHA_BLOCKSIZE;
    unsigned int rem = len % SHA_BLOCKSIZE;
    uint32_t count_lo = len << 3; /* 64-bit bit count */
    uint32_t count_hi = len >> 29;

    /* sha_init() */
    d[0] = 0x67452301u;
    d[1] = 0xefcdab89u;
    d[2] = 0x98badcfeu;
    d[3] = 0x10325476u;
    d[4] = 0xc3d2e1f0u;

    /* sha_update(): complete blocks */
    for (unsigned int b = 0; b < SHA_MAX_LEN / SHA_BLOCKSIZE; b++) {
        if (b < nfull) {
            for (int i = 0; i < SHA_BLOCKSIZE; i++)
                block[i] = msg[b * SHA_BLOCKSIZE + i];
            sha_transform(d, block);
        }
    }

    /* sha_final(): remaining bytes, 0x80 marker, zero pad and length */
    for (int i = 0; i < SHA_BLOCKSIZE; i++)
        block[i] = (i < (int)rem) ? msg[nfull * SHA_BLOCKSIZE + i] : 0;
    block[rem] = 0x80;
    if (rem + 1 > 56) {
        sha_transform(d, block);
        for (int i = 0; i < SHA_BLOCKSIZE; i++)
            block[i] = 0;
    }
    block[56] = (uint8_t)(count_hi >> 24);
    block[57] = (uint8_t)(count_hi >> 16);
    block[58] = (uint8_t)(count_hi >> 8);
    block[59] = (uint8_t)(count_hi);
    block[60] = (uint8_t)(count_lo >> 24);
    block[61] = (uint8_t)(count_lo >> 16);
    block[62] = (uint8_t)(count_lo >> 8);
    block[63] = (uint8_t)(count_lo);
    sha_transform(d, block);

    for (int i = 0; i < 5; i++)
        digest[i] = d[i];
}
