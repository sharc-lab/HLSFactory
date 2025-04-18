#ifndef CANNY_H
#define CANNY_H

#include "Sobel.h"
#include <math.h>
#include <array>
using namespace std;
//using namespace hls;
// for guassian operator matrix generation

#define SIGMA 3
#define PI 3.14159265358979323846

//bool convol(int array[H][W], int sizeImg[2], int* op, int sizeOp[2], int);
//bool gausFilter(int array[H][W]);
void gradientForm(int array[H][W], int);
int angle_class(double);
void nms(int magArray[H][W], int dirArray[H][W]);
void thresHolding(int magArray[H][W], bool, int);
int myMax(int, int, int);
//bool fill(int array1[H][W], int array2[H][W], int div, int reduce);
void histoBuild(int array[H][W]);

extern int magGrad[H][W];
extern int magGradY[H][W];
extern int magGradX[H][W];
extern int magGradOut[H][W];
extern int dirGrad[H][W];
extern int magGradOut5[H][W];
extern int magGradOut3[H][W];
extern int magGradOut1[H][W];

#endif
