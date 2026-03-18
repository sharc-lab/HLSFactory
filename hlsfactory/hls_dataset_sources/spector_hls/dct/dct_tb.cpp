#include "params.h"
#include <stdio.h>

int main() {
    int src[size];
    int dst[size];
    for (int i = 0; i < size; i++)
        src[i] = i % 256;
    for (int i = 0; i < size; i++)
        dst[i] = 0;
    DCT(dst, src);
    printf("DCT csim done\n");
    return 0;
}
