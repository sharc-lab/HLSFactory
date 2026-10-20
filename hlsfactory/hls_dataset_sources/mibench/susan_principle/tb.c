#include <stdint.h>
#include <stdio.h>
#include "susan.h"
#include "susan_image.h"

/* FNV-1a of the response map computed by the original MiBench
 * susan_principle() on the same image (little endian int array). */
static uint32_t fnv(const unsigned char *c, int n)
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
    static int r[SUSAN_X_SIZE * SUSAN_Y_SIZE];
    long sum = 0;

    susan_principle(input_image, r);

    for (int i = 0; i < SUSAN_X_SIZE * SUSAN_Y_SIZE; i++)
        sum += r[i];

    if (sum != 870085L ||
        fnv((const unsigned char *)r, sizeof(r)) != 0x3b0b2a57u) {
        printf("FAIL: response sum %ld hash %08x\n", sum,
               fnv((const unsigned char *)r, sizeof(r)));
        return 1;
    }
    printf("PASS\n");
    return 0;
}
