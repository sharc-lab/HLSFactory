#ifndef CONFIG_H_
#define CONFIG_H_

#include "common.h"

#define PI 314 //159265358979323846

#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X) // [0-255]


int
rad_to_deg(int rad);

int
pow_func(int base, int expo);

void
de_quantization(int *data_in, int *table, int *data_out);

void
idct(int *data_in, int *data_out);

void
data_shift_up(int *data_in, unsigned char *data_out);

void
decode_block(unsigned char *data_in,
             int data_size,
             int width,
             int height,
             unsigned char *data_out);

#endif /* CONFIG_H_ */

