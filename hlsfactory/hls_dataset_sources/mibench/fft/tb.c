#include <math.h>
#include <stdio.h>
#include "fft.h"

#define NWAVES 4

int main(void)
{
    static float re[FFT_NUM_SAMPLES], im[FFT_NUM_SAMPLES];
    static float fre[FFT_NUM_SAMPLES], fim[FFT_NUM_SAMPLES];
    static float bre[FFT_NUM_SAMPLES], bim[FFT_NUM_SAMPLES];
    float coeff[NWAVES], amp[NWAVES];
    unsigned s = 1;
    int errors = 0;

    /* sum of random sinusoids, as in the MiBench driver (main.c) */
    for (int i = 0; i < NWAVES; i++) {
        s = s * 1103515245u + 12345u;
        coeff[i] = (float)((s >> 16) % 1000);
        s = s * 1103515245u + 12345u;
        amp[i] = (float)((s >> 16) % 1000);
    }
    for (int i = 0; i < FFT_NUM_SAMPLES; i++) {
        re[i] = 0;
        im[i] = 0;
        for (int j = 0; j < NWAVES; j++) {
            s = s * 1103515245u + 12345u;
            if ((s >> 16) & 1)
                re[i] += coeff[j] * (float)cos(amp[j] * i);
            else
                re[i] += coeff[j] * (float)sin(amp[j] * i);
        }
    }

    fft_float(0, re, im, fre, fim);

    /* compare a few bins against a direct DFT (this library's forward transform uses the e^{+j} kernel) */
    double maxmag = 0;
    for (int k = 0; k < FFT_NUM_SAMPLES; k++) {
        double m = hypot(fre[k], fim[k]);
        if (m > maxmag)
            maxmag = m;
    }
    for (int k = 0; k < FFT_NUM_SAMPLES; k += 37) {
        double sr = 0, si = 0;
        for (int t = 0; t < FFT_NUM_SAMPLES; t++) {
            double a = 2.0 * M_PI * (double)((long)k * t % FFT_NUM_SAMPLES) / FFT_NUM_SAMPLES;
            sr += re[t] * cos(a);
            si += re[t] * sin(a);
        }
        if (fabs(sr - fre[k]) > 1e-3 * maxmag || fabs(si - fim[k]) > 1e-3 * maxmag) {
            printf("bin %d: got (%f,%f) expected (%f,%f)\n", k, fre[k], fim[k], sr, si);
            errors++;
        }
    }

    /* inverse transform must recover the input */
    fft_float(1, fre, fim, bre, bim);
    for (int i = 0; i < FFT_NUM_SAMPLES; i++) {
        if (fabs(bre[i] - re[i]) > 1e-2 * (1.0 + maxmag / FFT_NUM_SAMPLES) || fabs(bim[i]) > 1e-1) {
            printf("sample %d: roundtrip (%f,%f) vs %f\n", i, bre[i], bim[i], re[i]);
            errors++;
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
