#include <stdint.h>
#include <stdio.h>
#include "susan.h"
#include "susan_image.h"

/* FNV-1a of the image written by the original MiBench "susan input_small.pgm
 * out -e" (edge response, thinning and edge drawing) on the same input. */
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
    int count = susan_edges(input_image, out);

    if (count != 190 || fnv(out, SUSAN_X_SIZE * SUSAN_Y_SIZE) != 0xd4e98185u) {
        printf("FAIL: %d edge points, image hash %08x\n", count,
               fnv(out, SUSAN_X_SIZE * SUSAN_Y_SIZE));
        return 1;
    }
    printf("PASS\n");
    return 0;
}
