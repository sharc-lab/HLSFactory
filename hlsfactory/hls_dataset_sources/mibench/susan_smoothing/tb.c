#include <stdint.h>
#include <stdio.h>
#include "susan.h"
#include "susan_image.h"

/* FNV-1a of the output computed by the original MiBench susan_smoothing()
 * (susan input_small.pgm out -s) on the same image. */
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
    static uchar out[SUSAN_X_SIZE * SUSAN_Y_SIZE];

    susan_smoothing(input_image, out);

    if (fnv(out, SUSAN_X_SIZE * SUSAN_Y_SIZE) != 0x23d7b582u) {
        printf("FAIL: smoothed image hash %08x\n", fnv(out, SUSAN_X_SIZE * SUSAN_Y_SIZE));
        return 1;
    }
    printf("PASS\n");
    return 0;
}
