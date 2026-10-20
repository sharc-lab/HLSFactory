#include <stdint.h>
#include <stdio.h>
#include "susan.h"
#include "susan_image.h"

/* FNV-1a of the image written by the original MiBench "susan input_small.pgm
 * out -c" (corner detection and drawing) on the same input; it finds 6 corners. */
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
    int count = susan_corners(input_image, out);

    if (count != 6 || fnv(out, SUSAN_X_SIZE * SUSAN_Y_SIZE) != 0x20e94065u) {
        printf("FAIL: %d corners, image hash %08x\n", count,
               fnv(out, SUSAN_X_SIZE * SUSAN_Y_SIZE));
        return 1;
    }
    printf("PASS\n");
    return 0;
}
