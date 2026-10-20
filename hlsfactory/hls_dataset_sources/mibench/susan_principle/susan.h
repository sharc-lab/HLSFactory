#ifndef SUSAN_H
#define SUSAN_H

#define SUSAN_X_SIZE 76
#define SUSAN_Y_SIZE 95

typedef unsigned char uchar;

/*
 * SUSAN edge strength map (susan -e -p, "principle") with the default
 * parameters of the benchmark: brightness threshold bt = 20 and geometric
 * threshold max_no_edges = 2650 on the 37 pixel circular mask.  The result r[]
 * holds the edge response (0 where the USAN area exceeds the threshold).
 */
void susan_principle(const uchar in[SUSAN_X_SIZE * SUSAN_Y_SIZE],
                    int r[SUSAN_X_SIZE * SUSAN_Y_SIZE]);

#endif
