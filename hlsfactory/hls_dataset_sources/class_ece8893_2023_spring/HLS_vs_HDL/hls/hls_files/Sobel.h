#ifndef SOBEL_H
#define SOBEL_H

#include <iostream>
#include <math.h>
#include <array>
#include "utils.h"

void convol(int array[H * W], int sizeImg[2], int* op, int sizeOp[2], int, int tmpArray[H][W]);
void fill(int array1[H][W], int array2[H][W], int div, int reduce);

#endif
