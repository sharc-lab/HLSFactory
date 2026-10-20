/**********************************************************************/
/*  SUSAN Version 2l by Stephen Smith                                 */
/*  Oxford Centre for Functional Magnetic Resonance Imaging of the    */
/*  Brain, Department of Clinical Neurology, Oxford University, UK    */
/*  (Previously at DRA Farnborough, UK.)                              */
/*  Email: steve@fmrib.ox.ac.uk                                       */
/*  See the original susan.c of MiBench automotive/susan for the      */
/*  full copyright and licence statement.                             */
/**********************************************************************/
/*
 * MiBench automotive/susan: susan_smoothing(), setup_brightness_lut(),
 * median() and enlarge() for the default "-s" mode.
 *
 * HLS adaptation: static image buffers instead of malloc, fixed image and
 * mask size, 2-D index arithmetic instead of pointer walking, no file I/O.
 */
#include <math.h>
#include "susan.h"

#define BT 20            /* brightness threshold */
#define DT 4.0f          /* distance threshold */
#define MASK_SIZE 7      /* ((int)(1.5 * DT)) + 1 */
#define MASK_N (2 * MASK_SIZE + 1)
#define BIG_X (SUSAN_X_SIZE + 2 * MASK_SIZE)
#define BIG_Y (SUSAN_Y_SIZE + 2 * MASK_SIZE)

typedef int TOTAL_TYPE;

/* brightness look-up table: lut[k + 256] for k in [-256, 256] */
static void setup_brightness_lut(uchar lut[513], int thresh)
{
    for (int k = -256; k < 257; k++) {
        float temp = ((float)k) / ((float)thresh);
        temp = temp * temp;
        temp = 100.0f * exp(-temp);
        lut[k + 256] = (uchar)temp;
    }
}

/* median of the 8 neighbours of (i, j) */
static uchar median(const uchar in[BIG_Y][BIG_X], int i, int j)
{
    int p[8], tmp;

    p[0] = in[i - 1][j - 1];
    p[1] = in[i - 1][j];
    p[2] = in[i - 1][j + 1];
    p[3] = in[i][j - 1];
    p[4] = in[i][j + 1];
    p[5] = in[i + 1][j - 1];
    p[6] = in[i + 1][j];
    p[7] = in[i + 1][j + 1];

    for (int k = 0; k < 7; k++)
        for (int l = 0; l < (7 - k); l++)
            if (p[l] > p[l + 1]) {
                tmp = p[l];
                p[l] = p[l + 1];
                p[l + 1] = tmp;
            }

    return (uchar)((p[3] + p[4]) / 2);
}

/* enlarge the image so that borders can be dealt with easily (mirrored) */
static void enlarge(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE], uchar big[BIG_Y][BIG_X])
{
    const int border = MASK_SIZE;

    for (int i = 0; i < SUSAN_Y_SIZE; i++)
        for (int j = 0; j < SUSAN_X_SIZE; j++)
            big[i + border][j + border] = in[i * SUSAN_X_SIZE + j];

    /* top and bottom rows; mirrored */
    for (int i = 0; i < border; i++)
        for (int j = 0; j < SUSAN_X_SIZE; j++) {
            big[border - 1 - i][j + border] = in[i * SUSAN_X_SIZE + j];
            big[SUSAN_Y_SIZE + border + i][j + border] = in[(SUSAN_Y_SIZE - i - 1) * SUSAN_X_SIZE + j];
        }

    /* left and right columns */
    for (int i = 0; i < border; i++)
        for (int j = 0; j < SUSAN_Y_SIZE + 2 * border; j++) {
            big[j][border - 1 - i] = big[j][border + i];
            big[j][SUSAN_X_SIZE + border + i] = big[j][SUSAN_X_SIZE + border - 1 - i];
        }
}

void susan_smoothing(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE],
                     uchar out[SUSAN_X_SIZE * SUSAN_Y_SIZE])
{
    static uchar big[BIG_Y][BIG_X];
    uchar bp[513];
    uchar dp[MASK_N][MASK_N];
    int outp = 0;

    setup_brightness_lut(bp, BT);
    enlarge(in, big);

    /* distance look-up table */
    {
        float temp = -(DT * DT);
        for (int i = -MASK_SIZE; i <= MASK_SIZE; i++)
            for (int j = -MASK_SIZE; j <= MASK_SIZE; j++) {
                int x = (int)(100.0 * exp(((float)((i * i) + (j * j))) / temp));
                dp[i + MASK_SIZE][j + MASK_SIZE] = (uchar)x;
            }
    }

    for (int i = MASK_SIZE; i < BIG_Y - MASK_SIZE; i++) {
        for (int j = MASK_SIZE; j < BIG_X - MASK_SIZE; j++) {
            int area = 0;
            TOTAL_TYPE total = 0;
            int centre = big[i][j];

            for (int y = 0; y < MASK_N; y++) {
                for (int x = 0; x < MASK_N; x++) {
                    int brightness = big[i - MASK_SIZE + y][j - MASK_SIZE + x];
                    int tmp = dp[y][x] * bp[centre - brightness + 256];
                    area += tmp;
                    total += tmp * brightness;
                }
            }
            int tmp = area - 10000;
            if (tmp == 0)
                out[outp++] = median(big, i, j);
            else
                out[outp++] = (uchar)((total - (centre * 10000)) / tmp);
        }
    }
}
