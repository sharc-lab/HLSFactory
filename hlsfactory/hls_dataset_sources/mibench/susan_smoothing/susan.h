#ifndef SUSAN_H
#define SUSAN_H

#define SUSAN_X_SIZE 76
#define SUSAN_Y_SIZE 95

typedef unsigned char uchar;

/*
 * SUSAN structure-preserving noise reduction (susan -s) with the default
 * parameters of the benchmark: Gaussian mask with distance threshold
 * dt = 4.0 (15x15 mask) and brightness threshold bt = 20.
 */
void susan_smoothing(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE],
                     uchar out[SUSAN_X_SIZE * SUSAN_Y_SIZE]);

#endif
