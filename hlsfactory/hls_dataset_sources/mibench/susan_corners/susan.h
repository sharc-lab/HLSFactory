#ifndef SUSAN_H
#define SUSAN_H

#define SUSAN_X_SIZE 76
#define SUSAN_Y_SIZE 95
#define SUSAN_MAX_CORNERS 256 /* capacity of the corner list (the original uses 15000) */

typedef unsigned char uchar;

/*
 * SUSAN corner detection (susan -c) with the default parameters of the
 * benchmark: brightness threshold bt = 20, geometric threshold
 * max_no_corners = 1850, 7x7 non-maximum suppression, drawing mode 0
 * (white 3x3 block with a black centre on each corner).  out is the input
 * image with the corners drawn on it, exactly the image the benchmark
 * writes.  Returns the number of corners found.
 */
int susan_corners(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE],
                  uchar out[SUSAN_X_SIZE * SUSAN_Y_SIZE]);

#endif
