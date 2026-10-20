#ifndef SHA_H
#define SHA_H

#include <stdint.h>

#define SHA_BLOCKSIZE 64
#define SHA_DIGESTSIZE 20
#define SHA_MAX_LEN 1024 /* longest message handled by sha_digest() */

/*
 * NIST Secure Hash Algorithm over msg[0..len-1] (len <= SHA_MAX_LEN).
 * As in MiBench this computes the original SHA unless USE_MODIFIED_SHA is
 * defined (NIST's 7/11/94 modification, i.e. SHA-1).
 */
void sha_digest(const uint8_t msg[SHA_MAX_LEN], unsigned int len, uint32_t digest[5]);

#endif
