#ifndef SUSAN_H
#define SUSAN_H

#define SUSAN_X_SIZE 76
#define SUSAN_Y_SIZE 95

typedef unsigned char uchar;

/*
 * SUSAN edge detection (susan -e) with the default parameters of the
 * benchmark: brightness threshold bt = 20, geometric threshold
 * max_no_edges = 2650, thinning enabled and white 3x3 edge marking
 * (drawing mode 0).  out is the input image with the detected edge points
 * drawn on it, exactly the image the benchmark writes.  Returns the number
 * of edge points.
 */
int susan_edges(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE],
                uchar out[SUSAN_X_SIZE * SUSAN_Y_SIZE]);

#endif
