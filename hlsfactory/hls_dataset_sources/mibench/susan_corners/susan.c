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
 * MiBench automotive/susan: setup_brightness_lut(), susan_corners() and
 * corner_draw() for the default "-c" mode.
 *
 * HLS adaptation: static buffers instead of malloc, fixed image size, a
 * fixed-capacity corner list, and the unrolled pointer walks over the 37
 * pixel circular mask are loops over the mask (the gradient weights of the
 * original statements are dx and dy).  The "n < max_no" early outs of the
 * original are folded into one test after the full sum, which is
 * equivalent because the terms are never negative.  The per-corner
 * direction (cgx, cgy) stored in the original's list is not used by the
 * drawing and is omitted.  No file I/O.
 */
#include <math.h>
#include <stdlib.h>
#include "susan.h"

#define BT 20            /* brightness threshold */
#define MAX_NO 1850      /* geometric threshold (max_no_corners) */
#define x_size SUSAN_X_SIZE
#define y_size SUSAN_Y_SIZE
#define NPIX (SUSAN_X_SIZE * SUSAN_Y_SIZE)

#define FTOI(a) ((a) < 0 ? ((int)(a - 0.5)) : ((int)(a + 0.5)))

/* brightness look-up table: lut[k + 256] for k in [-256, 256], "form 6" */
static void setup_brightness_lut(uchar lut[513], int thresh)
{
    for (int k = -256; k < 257; k++) {
        float temp = ((float)k) / ((float)thresh);
        temp = temp * temp;
        temp = temp * temp * temp;
        temp = 100.0f * exp(-temp);
        lut[k + 256] = (uchar)temp;
    }
}

/* half width of each row of the 37 pixel circular mask, rows -3..+3 */
static const int half_width[7] = {1, 2, 3, 3, 3, 2, 1};

int susan_corners(const uchar in[NPIX], uchar out[NPIX])
{
    static int r[NPIX];
    static int corner_x[SUSAN_MAX_CORNERS];
    static int corner_y[SUSAN_MAX_CORNERS];
    uchar bp[513];
    const int max_no = MAX_NO;
    int n, x, y, sq, xx, yy, i, j;
    float divide;
    int count = 0;

    setup_brightness_lut(bp, BT);

    for (i = 0; i < NPIX; i++) {
        r[i] = 0;
        out[i] = in[i];
    }

    for (i = 5; i < y_size - 5; i++)
        for (j = 5; j < x_size - 5; j++) {
            int centre = in[i * x_size + j];

            n = 100;
            for (int dy = -3; dy <= 3; dy++)
                for (int dx = -3; dx <= 3; dx++)
                    if (dx >= -half_width[dy + 3] && dx <= half_width[dy + 3] && !(dy == 0 && dx == 0))
                        n += bp[centre - in[(i + dy) * x_size + j + dx] + 256];

            if (n < max_no) {
                x = 0;
                y = 0;
                for (int dy = -3; dy <= 3; dy++)
                    for (int dx = -3; dx <= 3; dx++)
                        if (dx >= -half_width[dy + 3] && dx <= half_width[dy + 3] && !(dy == 0 && dx == 0)) {
                            int c = bp[centre - in[(i + dy) * x_size + j + dx] + 256];
                            x += c * dx;
                            y += c * dy;
                        }

                xx = x * x;
                yy = y * y;
                sq = xx + yy;
                if (sq > ((n * n) / 2)) {
                    if (yy < xx) {
                        divide = (float)y / (float)abs(x);
                        sq = abs(x) / x;
                        sq = bp[centre - in[(i + FTOI(divide)) * x_size + j + sq] + 256] +
                             bp[centre - in[(i + FTOI(2 * divide)) * x_size + j + 2 * sq] + 256] +
                             bp[centre - in[(i + FTOI(3 * divide)) * x_size + j + 3 * sq] + 256];
                    } else {
                        divide = (float)x / (float)abs(y);
                        sq = abs(y) / y;
                        sq = bp[centre - in[(i + sq) * x_size + j + FTOI(divide)] + 256] +
                             bp[centre - in[(i + 2 * sq) * x_size + j + FTOI(2 * divide)] + 256] +
                             bp[centre - in[(i + 3 * sq) * x_size + j + FTOI(3 * divide)] + 256];
                    }

                    if (sq > 290) {
                        r[i * x_size + j] = max_no - n;
                    }
                }
            }
        }

    /* to locate the local maxima: 7x7 non-maximum suppression (SEVEN_SUPP) */
    for (i = 5; i < y_size - 5; i++)
        for (j = 5; j < x_size - 5; j++) {
            x = r[i * x_size + j];
            if (x > 0) {
                int is_max = 1;
                for (int dy = -3; dy <= 3; dy++)
                    for (int dx = -3; dx <= 3; dx++) {
                        int v = r[(i + dy) * x_size + j + dx];
                        if (dy == 0 && dx == 0)
                            continue;
                        /* strictly greater than everything before, greater or equal after (raster order) */
                        if (dy < 0 || (dy == 0 && dx < 0)) {
                            if (!(x > v))
                                is_max = 0;
                        } else {
                            if (!(x >= v))
                                is_max = 0;
                        }
                    }
                if (is_max && count < SUSAN_MAX_CORNERS) {
                    corner_x[count] = j;
                    corner_y[count] = i;
                    count++;
                }
            }
        }

    /* corner_draw(), drawing mode 0: white 3x3 block with a black centre */
    for (int k = 0; k < SUSAN_MAX_CORNERS; k++) {
        if (k < count) {
            int base = (corner_y[k]) * x_size + corner_x[k];
            for (int dy = -1; dy <= 1; dy++)
                for (int dx = -1; dx <= 1; dx++)
                    out[base + dy * x_size + dx] = (dy == 0 && dx == 0) ? 0 : 255;
        }
    }

    return count;
}
