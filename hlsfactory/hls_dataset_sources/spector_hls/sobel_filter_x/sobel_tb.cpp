#include "sobel.h"
#include "params.h"
#include <stdio.h>

int main() {
    ap_uint<8> input_image[H][W];
    ap_uint<1> output_image[H][W];
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            input_image[i][j] = (i + j) % 256;
    for (int i = 0; i < H; i++)
        for (int j = 0; j < W; j++)
            output_image[i][j] = 0;
    sobel_x(input_image, output_image);
    printf("sobel_x csim done\n");
    return 0;
}
