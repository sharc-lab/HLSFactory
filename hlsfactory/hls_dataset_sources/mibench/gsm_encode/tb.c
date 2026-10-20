#include <stdio.h>
#include <string.h>
#include "gsm_top.h"
#include "gsm_vectors.h"

int main(void)
{
    unsigned char out[GSM_FRAMES * GSM_FRAME_BYTES];
    int errors = 0;

    gsm_encode_block(input_pcm, out);

    for (int i = 0; i < GSM_FRAMES * GSM_FRAME_BYTES; i++) {
        if (out[i] != expected_frames[i]) {
            printf("byte %d (frame %d): got %02x expected %02x\n", i, i / GSM_FRAME_BYTES,
                   out[i], expected_frames[i]);
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
