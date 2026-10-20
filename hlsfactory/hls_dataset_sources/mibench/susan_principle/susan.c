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
 * MiBench automotive/susan: setup_brightness_lut() and susan_principle(),
 * the edge/corner response stage shared by "susan -e" and "susan -c".
 *
 * HLS adaptation: static buffers instead of malloc, 2-D index arithmetic
 * with an explicit mask table instead of the unrolled pointer walk, no
 * file I/O.
 */
#include <math.h>
#include "susan.h"

#define BT 20            /* brightness threshold */
#define MAX_NO 2650      /* geometric threshold (max_no_edges) */

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

void susan_principle(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE],
                     int r[SUSAN_X_SIZE * SUSAN_Y_SIZE])
{
    /* half width of each row of the 37 pixel circular mask, rows -3..+3 */
    static const int half_width[7] = {1, 2, 3, 3, 3, 2, 1};
    uchar bp[513];

    setup_brightness_lut(bp, BT);

    for (int i = 0; i < SUSAN_X_SIZE * SUSAN_Y_SIZE; i++)
        r[i] = 0;

    for (int i = 3; i < SUSAN_Y_SIZE - 3; i++) {
        for (int j = 3; j < SUSAN_X_SIZE - 3; j++) {
            int n = 100;
            int centre = in[i * SUSAN_X_SIZE + j];

            for (int dy = -3; dy <= 3; dy++) {
                int w = half_width[dy + 3];
                for (int dx = -3; dx <= 3; dx++) {
                    if (dx >= -w && dx <= w && !(dy == 0 && dx == 0)) {
                        int pix = in[(i + dy) * SUSAN_X_SIZE + (j + dx)];
                        n += bp[centre - pix + 256];
                    }
                }
            }

            if (n <= MAX_NO)
                r[i * SUSAN_X_SIZE + j] = MAX_NO - n;
        }
    }
}
