#include <stdio.h>
#include "gsm_top.h"
#include "gsm_vectors.h"

int main(void)
{
    short pcm[GSM_FRAMES * GSM_FRAME_SAMPLES];
    int errors = 0;

    if (gsm_decode_block(input_frames, pcm) != 0) {
        printf("decoder rejected a frame\n");
        errors++;
    }

    for (int i = 0; i < GSM_FRAMES * GSM_FRAME_SAMPLES; i++) {
        if (pcm[i] != expected_pcm[i]) {
            printf("sample %d: got %d expected %d\n", i, pcm[i], expected_pcm[i]);
            if (++errors > 10)
                break;
        }
    }

    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
