#include <stdio.h>
#include <string.h>
#include "sha.h"

/* Expected digests computed with an independent implementation of the
 * original SHA (SHA-0) and of NIST's modified SHA (SHA-1, USE_MODIFIED_SHA). */
#ifdef USE_MODIFIED_SHA
static const uint32_t exp_abc[5] = {0xa9993e36, 0x4706816a, 0xba3e2571, 0x7850c26c, 0x9cd0d89d};
static const uint32_t exp_1000[5] = {0x4231a8a5, 0x0a10fa97, 0x58db8ec7, 0x1fdef855, 0xb751048a};
#else
static const uint32_t exp_abc[5] = {0x0164b8a9, 0x14cd2a5e, 0x74c4f7ff, 0x082c4d97, 0xf1edf880};
static const uint32_t exp_1000[5] = {0x4e434dd5, 0xca402e03, 0xa397f8de, 0xd74d8f94, 0x509cf5ad};
#endif

static int check(const char *name, const uint8_t *msg, unsigned int len, const uint32_t *exp)
{
    static uint8_t buf[SHA_MAX_LEN];
    uint32_t digest[5];

    memset(buf, 0, sizeof(buf));
    memcpy(buf, msg, len);
    sha_digest(buf, len, digest);
    if (memcmp(digest, exp, sizeof(digest)) != 0) {
        printf("%s: got %08x %08x %08x %08x %08x\n", name, digest[0], digest[1],
               digest[2], digest[3], digest[4]);
        return 1;
    }
    return 0;
}

int main(void)
{
    uint8_t msg[1000];
    int errors = 0;

    for (int i = 0; i < 1000; i++)
        msg[i] = (uint8_t)((i * 7 + 3) & 0xff);

    errors += check("abc", (const uint8_t *)"abc", 3, exp_abc);
    errors += check("1000 bytes", msg, 1000, exp_1000);

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
