#include "params.h"
#include <stdio.h>

int main() {
    unsigned char data[DATA_SIZE / KNOB_NUM_WORK_GROUPS];
    unsigned int histogram[KNOB_HIST_SIZE];
    for (unsigned long i = 0; i < DATA_SIZE / KNOB_NUM_WORK_GROUPS; i++)
        data[i] = (unsigned char)(i % 256);
    for (int i = 0; i < KNOB_HIST_SIZE; i++)
        histogram[i] = 0;
    histogram_hls(data, histogram, 0);
    printf("histogram csim done\n");
    return 0;
}
