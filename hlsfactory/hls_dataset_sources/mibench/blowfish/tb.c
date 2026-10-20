#include <stdio.h>
#include <string.h>
#include "blowfish.h"

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
    /* MiBench runme_small.sh key: 1234567890abcdeffedcba0987654321 */
    static const uint8_t key[BF_KEY_LEN] = {0x12, 0x34, 0x56, 0x78, 0x90, 0xab, 0xcd, 0xef,
                                            0xfe, 0xdc, 0xba, 0x09, 0x87, 0x65, 0x43, 0x21};
    static const uint8_t iv[8] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77};
    static uint8_t plain[BF_DATA_LEN], enc[BF_DATA_LEN], dec[BF_DATA_LEN];
    int errors = 0;

    /* Eric Young's test vector: key 0000000000000000, plaintext 0 -> 4EF997456198DD78 */
    {
        BF_KEY ks;
        uint8_t zero_key[8] = {0};
        uint32_t blk[2] = {0, 0};
        BF_set_key(&ks, 8, zero_key);
        BF_encrypt(blk, &ks);
        if (blk[0] != 0x4EF99745u || blk[1] != 0x6198DD78u) {
            printf("ECB test vector mismatch: %08x%08x\n", blk[0], blk[1]);
            errors++;
        }
    }

    for (int i = 0; i < BF_DATA_LEN; i++)
        plain[i] = (uint8_t)((i * 13 + 5) & 0xff);

    blowfish_cfb64(key, iv, plain, enc, 1);
    /* reference ciphertext from OpenSSL: openssl enc -bf-cfb -K <key> -iv <iv> */
    if (fnv(enc, BF_DATA_LEN) != 0x48e72bc1u || enc[0] != 0xf4 || enc[7] != 0x30) {
        printf("CFB ciphertext mismatch (hash %08x)\n", fnv(enc, BF_DATA_LEN));
        errors++;
    }

    blowfish_cfb64(key, iv, enc, dec, 0);
    if (memcmp(plain, dec, BF_DATA_LEN) != 0) {
        printf("decrypt does not recover plaintext\n");
        errors++;
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
