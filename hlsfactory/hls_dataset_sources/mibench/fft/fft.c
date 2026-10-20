/*============================================================================

    fourierf.c  -  Don Cross <dcross@intersrv.com>

    http://www.intersrv.com/~dcross/fft.html

    Contains definitions for doing Fourier transforms
    and inverse Fourier transforms.

    This module performs operations on arrays of 'float'.

    Revision history:

1998 September 19 [Don Cross]
    Updated coding standards.
    Improved efficiency of trig calculations.

============================================================================*/
/*
 * MiBench telecomm/FFT.
 *
 * HLS adaptation: fixed sample count, the argument checks / CheckPointer
 * (which call exit()) are dropped, and ReverseBits is the plain loop from
 * fftmisc.c.
 */
#include <math.h>
#include "fft.h"

#define DDC_PI (3.14159265358979323846)

static unsigned ReverseBits(unsigned index, unsigned NumBits)
{
    unsigned i, rev;

    for (i = rev = 0; i < NumBits; i++) {
        rev = (rev << 1) | (index & 1);
        index >>= 1;
    }
    return rev;
}

void fft_float(int inverse_transform,
               const float RealIn[FFT_NUM_SAMPLES], const float ImagIn[FFT_NUM_SAMPLES],
               float RealOut[FFT_NUM_SAMPLES], float ImagOut[FFT_NUM_SAMPLES])
{
    unsigned i, j, k, n;
    unsigned BlockSize, BlockEnd;

    double angle_numerator = 2.0 * DDC_PI;
    double tr, ti; /* temp real, temp imaginary */

    if (inverse_transform)
        angle_numerator = -angle_numerator;

    /*
    **   Do simultaneous data copy and bit-reversal ordering into outputs...
    */
    for (i = 0; i < FFT_NUM_SAMPLES; i++) {
        j = ReverseBits(i, FFT_NUM_BITS);
        RealOut[j] = RealIn[i];
        ImagOut[j] = ImagIn[i];
    }

    /*
    **   Do the FFT itself...
    */
    BlockEnd = 1;
    for (BlockSize = 2; BlockSize <= FFT_NUM_SAMPLES; BlockSize <<= 1) {
        double delta_angle = angle_numerator / (double)BlockSize;
        double sm2 = sin(-2 * delta_angle);
        double sm1 = sin(-delta_angle);
        double cm2 = cos(-2 * delta_angle);
        double cm1 = cos(-delta_angle);
        double w = 2 * cm1;
        double ar[3], ai[3];

        for (i = 0; i < FFT_NUM_SAMPLES; i += BlockSize) {
            ar[2] = cm2;
            ar[1] = cm1;

            ai[2] = sm2;
            ai[1] = sm1;

            for (j = i, n = 0; n < BlockEnd; j++, n++) {
                ar[0] = w * ar[1] - ar[2];
                ar[2] = ar[1];
                ar[1] = ar[0];

                ai[0] = w * ai[1] - ai[2];
                ai[2] = ai[1];
                ai[1] = ai[0];

                k = j + BlockEnd;
                tr = ar[0] * RealOut[k] - ai[0] * ImagOut[k];
                ti = ar[0] * ImagOut[k] + ai[0] * RealOut[k];

                RealOut[k] = RealOut[j] - tr;
                ImagOut[k] = ImagOut[j] - ti;

                RealOut[j] += tr;
                ImagOut[j] += ti;
            }
        }

        BlockEnd = BlockSize;
    }

    /*
    **   Need to normalize if inverse transform...
    */
    if (inverse_transform) {
        double denom = (double)FFT_NUM_SAMPLES;

        for (i = 0; i < FFT_NUM_SAMPLES; i++) {
            RealOut[i] /= denom;
            ImagOut[i] /= denom;
        }
    }
}
