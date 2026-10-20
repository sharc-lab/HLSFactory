#include <stdio.h>
#include <string.h>
#include "aes.h"

static uint32_t fnv(const uint8_t *c, int n)
{
    uint32_t h = 2166136261u;
    while (n--) {
        h ^= *c++;
        h *= 16777619u;
    }
    return h;
}

int main(void)
{
    static uint8_t in[AES_DATA_LEN], enc[AES_DATA_LEN], dec[AES_DATA_LEN];
    int errors = 0;

    /* FIPS-197 appendix C.1 */
    {
        uint8_t key[AES_KEY_SIZE], pt[AES_DATA_LEN], ct[AES_DATA_LEN];
        static const uint8_t expect[16] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30,
                                           0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
        for (int i = 0; i < 16; i++)
            key[i] = (uint8_t)i;
        for (int i = 0; i < AES_DATA_LEN; i++)
            pt[i] = (uint8_t)(((i & 15) << 4) | (i & 15));
        rijndael_ecb(key, pt, ct, 0);
        if (memcmp(ct, expect, 16) != 0) {
            printf("FIPS-197 C.1 mismatch\n");
            errors++;
        }
    }

    /* 256 bytes under the FIPS-197 appendix B key, reference from OpenSSL
     * (openssl enc -aes-128-ecb -nopad) */
    {
        static const uint8_t key[AES_KEY_SIZE] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
                                                  0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
        for (int i = 0; i < AES_DATA_LEN; i++)
            in[i] = (uint8_t)((i * 29 + 11) & 0xff);

        rijndael_ecb(key, in, enc, 0);
        if (fnv(enc, AES_DATA_LEN) != 0x4c137b3du || enc[0] != 0x39 || enc[3] != 0x6f) {
            printf("ciphertext mismatch (hash %08x)\n", fnv(enc, AES_DATA_LEN));
            errors++;
        }
        rijndael_ecb(key, enc, dec, 1);
        if (memcmp(in, dec, AES_DATA_LEN) != 0) {
            printf("decrypt does not recover plaintext\n");
            errors++;
        }
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
