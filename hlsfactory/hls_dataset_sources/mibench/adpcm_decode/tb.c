#include <stdio.h>
#include <stdint.h>
#include "adpcm.h"

/* Expected hashes were produced by the original MiBench rawcaudio/rawdaudio
 * code (adpcm_coder/adpcm_decoder) on this exact input. */
static uint32_t fnv(const void *p, int n)
{
    const unsigned char *c = p;
    uint32_t h = 2166136261u;
    while (n--) {
        h ^= *c++;
        h *= 16777619u;
    }
    return h;
}

int main(void)
{
    static short in[ADPCM_NSAMPLES];
    static signed char enc[ADPCM_NSAMPLES / 2];
    static short dec[ADPCM_NSAMPLES];
    int state[2] = {0, 0};
    int errors = 0;

    /* triangle wave plus noise */
    uint32_t s = 777;
    int tri = 0, dir = 900;
    for (int i = 0; i < ADPCM_NSAMPLES; i++) {
        s = s * 1103515245u + 12345u;
        tri += dir;
        if (tri > 12000 || tri < -12000)
            dir = -dir;
        in[i] = (short)(tri + (int)((s >> 16) & 0x3ff) - 512);
    }

    /* the stream to decode comes from the (already verified) reference encoder */
    adpcm_coder(in, enc, state);
    state[0] = 0;
    state[1] = 0;
    adpcm_decoder(enc, dec, state);
    if (fnv(dec, ADPCM_NSAMPLES * sizeof(short)) != 0xd163f2d1u) {
        printf("decoded samples hash mismatch\n");
        errors++;
    }
    if (state[0] != -7355 || state[1] != 53) {
        printf("final state mismatch: %d %d\n", state[0], state[1]);
        errors++;
    }

    (void)dec;
    if (errors) {
        printf("FAIL: %d errors\n", errors);
        return 1;
    }
    printf("PASS\n");
    return 0;
}
