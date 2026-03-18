#include "fpga_temp_matching.h"
#include "params.h"
#include <stdio.h>

int main() {
    axis_t INPUT[size];
    axis_t OUTPUT[size];
    for (int i = 0; i < size; i++) {
        INPUT[i].data = (unsigned char)(i % 256);
        INPUT[i].last = 0;
    }
    INPUT[size - 1].last = 1;
    for (int i = 0; i < size; i++) {
        OUTPUT[i].data = 0;
        OUTPUT[i].last = 0;
    }
    SAD_MATCH(INPUT, OUTPUT);
    printf("SAD_MATCH csim done\n");
    return 0;
}
