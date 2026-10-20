#ifndef FFT_H
#define FFT_H

#define FFT_NUM_SAMPLES 1024 /* must be a power of two */
#define FFT_NUM_BITS 10      /* log2(FFT_NUM_SAMPLES) */

/*
 * Radix-2 FFT (inverse_transform = 0) or inverse FFT (inverse_transform = 1,
 * includes the 1/N normalisation) of a complex float sequence.
 */
void fft_float(int inverse_transform,
               const float RealIn[FFT_NUM_SAMPLES], const float ImagIn[FFT_NUM_SAMPLES],
               float RealOut[FFT_NUM_SAMPLES], float ImagOut[FFT_NUM_SAMPLES]);

#endif
