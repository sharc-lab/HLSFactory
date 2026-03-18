#include "fir_hls.h"
#include <stdio.h>

int main() {
    float dataPtr[INPUT_SIZE];
    float filterPtr[FILTER_SIZE];
    float resultPtr[RESULT_SIZE];
    for (unsigned int i = 0; i < INPUT_SIZE; i++)
        dataPtr[i] = 0.0f;
    for (unsigned int i = 0; i < FILTER_SIZE; i++)
        filterPtr[i] = (i % 2 == 0) ? 1.0f : 0.0f;
    for (unsigned int i = 0; i < RESULT_SIZE; i++)
        resultPtr[i] = 0.0f;
    fir_hls(dataPtr, filterPtr, resultPtr, PADDEDNUMINPUTPOINTS,
            PADDEDSINGLEINPUTLENGTH, TOTALFILTERSLENGTHKERNELARG);
    printf("fir csim done\n");
    return 0;
}
