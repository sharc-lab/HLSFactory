#include <stdio.h>
#include <string.h>
#include "crc32.h"

/* Bit-serial reference CRC-32 (polynomial 0xEDB88320) */
static crc_t crc32_ref(const uint8_t *buf, unsigned int len)
{
    crc_t crc = 0xFFFFFFFF;
    for (unsigned int i = 0; i < len; i++) {
        crc ^= buf[i];
        for (int b = 0; b < 8; b++)
            crc = (crc >> 1) ^ ((crc & 1) ? 0xEDB88320u : 0);
    }
    return ~crc;
}

int main(void)
{
    static uint8_t buf[CRC32_MAX_LEN];
    int errors = 0;

    /* standard check value */
    memset(buf, 0, sizeof(buf));
    memcpy(buf, "123456789", 9);
    if (crc32buf(buf, 9) != 0xCBF43926u) {
        printf("check value mismatch: %08x\n", crc32buf(buf, 9));
        errors++;
    }

    /* pseudo-random payloads of several lengths */
    uint32_t seed = 12345;
    for (int i = 0; i < CRC32_MAX_LEN; i++) {
        seed = seed * 1103515245u + 12345u;
        buf[i] = (uint8_t)(seed >> 16);
    }
    unsigned int lens[] = {0, 1, 7, 64, 255, 1000, CRC32_MAX_LEN};
    for (unsigned int k = 0; k < sizeof(lens) / sizeof(lens[0]); k++) {
        crc_t got = crc32buf(buf, lens[k]);
        crc_t exp = crc32_ref(buf, lens[k]);
        if (got != exp) {
            printf("len %u: got %08x expected %08x\n", lens[k], got, exp);
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
