#include "canny.h"

//global 2d array for writing
int magGrad[H][W];
int magGradX[H][W];
int magGradY[H][W];
int dirGrad[H][W];
int TmpConvArray[H][W];
int magGradOut[H][W];
int magGradOut5[H][W];
int magGradOut3[H][W];
int magGradOut1[H][W];

int countTable[256] = { 0 };
int OP_PEWITT_X[9] = { -1,0,1,-1,0,1,-1,0,1 };
int OP_PEWITT_Y[9] = { 1,1,1,0,0,0,-1,-1,-1 };
int OP_SOBEL_X[9] = { -1,0,1,-2,0,2,-1,0,1 };
int OP_SOBEL_Y[9] = { 1,2,1,0,0,0,-1,-2,-1 };

int OneDImgArray[H * W] = { 0 };
int removed = 0;

//
void gradientForm(int array[H][W], int opType)
{
	int rowArray = H;
	int colArray = W;
	int xGrad = 0;
	int yGrad = 0;

	if (opType == 0)
	{ //naive way for gradient magnitude computation Gy = y[j]-y[j-1]
		for (int i = 1; i < rowArray; i++)
		{
			//#pragma HLS UNROLL factor=512
			for (int j = 1; j < colArray; j++)
			{
				//#pragma HLS UNROLL factor=512
								//int sizeOp[2] = { GAUS_SIZE,GAUS_SIZE };
				xGrad = array[i][j] - array[i][j - 1];
				yGrad = array[i][j] - array[i - 1][j];
				magGradY[i][j] = yGrad;
				magGradX[i][j] = xGrad;
				magGrad[i][j] = sqrt(yGrad * yGrad + xGrad * xGrad);
				dirGrad[i][j] = angle_class(atan2(yGrad, xGrad) / PI * 180);
			}
		}
	}
	else if (opType == 1)
	{
		// We use pewitt operator and convlution to do gradient computing
		int sizeImg[2] = { H,W };
		int sizeOp[2] = { 3,3 };
		//convolution
		for (int i = 0; i < H; i++)
		{
			for (int j = 0; j < W; j++)
			{
				OneDImgArray[i * W + j] = array[i][j];
			}
		}
		//Gx
		convol(OneDImgArray, sizeImg, OP_PEWITT_X, sizeOp, 1, TmpConvArray);
		fill(TmpConvArray, magGradX, 1, 4);
		//Gy
		convol(OneDImgArray, sizeImg, OP_PEWITT_Y, sizeOp, 1,TmpConvArray);
		fill(TmpConvArray, magGradY, 1, 4);

		// make sure no pixel has a value larger than 255 (maximum of our greyscale)
		for (int i = 0; i < rowArray; i++)
		{
			//#pragma HLS UNROLL factor=512
			for (int j = 0; j < colArray; j++)
			{
				//#pragma HLS UNROLL factor=512
				if (abs(magGradX[i][j]) + abs(magGradY[i][j]) > 255) {
					magGrad[i][j] = 255;
				}
				else
				{
					//magnitude
					magGrad[i][j] = abs(magGradX[i][j]) + abs(magGradY[i][j]);
				}
				//direction
				dirGrad[i][j] = angle_class(atan2(magGradY[i][j], magGradX[i][j]) / PI * 180);
			}
		}
	}
}

//classify the angle into 4 classes
int angle_class(double angle)
{
	if ((angle < 22.5 && angle >= -22.5) || angle >= 157.5 || angle < -157.5)
	{
		return 0;
	}
	else if ((angle >= 22.5 && angle < 67.5) || (angle < -112.5 && angle >= -157.5))
	{
		return 1;
	}
	else if ((angle >= 67.5 && angle < 112.5) || (angle < -67.5 && angle >= -112.5))
	{
		return 2;
	}
	else if ((angle >= 112.5 && angle < 157.5) || (angle < -22.5 && angle >= -67.5))
	{
		return 3;
	}
	else
	{
		return 1;
	}
}

//Function: non maximum suppression
//all those trivial 'if' statement are for edge and corner cases
void nms(int magArray[H][W], int dirArray[H][W])
{

	size_t rowArray = H;
	size_t colArray = W;
	for (int i = 1; i < rowArray; i++)
	{
		for (int j = 1; j < colArray; j++)
		{
			switch (dirArray[i][j])
			{
			case 0:
			{
				if (j == 0)//beginning col
				{
					if (magArray[i][j] <= magArray[i][j + 1])
					{
						magGradOut[i][j] = 0;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else if (j == W - 1)
				{
					if (magArray[i][j] <= magArray[i][j - 1])
					{
						magGradOut[i][j] = 0;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else
				{
					if (myMax(magArray[i][j], magArray[i][j - 1], magArray[i][j + 1]) != magArray[i][j])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				break;
			}

			case 1:
			{
				if ((j == 0 && i == 0) || (j == W - 1 && i == W - 1))
				{
					magGradOut[i][j] = magArray[i][j];
				}
				else if ((j == 0 && i != 0) || (j != W - 1 && i == W - 1))//beginning col
				{
					if (magArray[i][j] <= magArray[i - 1][j + 1])
					{
						magGradOut[i][j] = 0;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else if ((j != 0 && i == 0) || (j == W - 1 && i != W - 1))
				{
					if (magArray[i][j] <= magArray[i + 1][j - 1])
					{
						magGradOut[i][j] = 0;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else
				{
					if (myMax(magArray[i][j], magArray[i + 1][j - 1], magArray[i - 1][j + 1]) != magArray[i][j])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				break;
			}

			case 2:
			{
				if (i == 0)//beginning col
				{
					if (magArray[i][j] <= magArray[i + 1][j])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else if (i == W - 1)
				{
					if (magArray[i][j] <= magArray[i - 1][j])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else
				{
					if (myMax(magArray[i][j], magArray[i - 1][j], magArray[i + 1][j]) != magArray[i][j])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				break;
			}

			case 3:
			{
				if ((j == W - 1 && i == 0) || (j == 0 && i == W - 1))
				{
					magGradOut[i][j] = magArray[i][j];
				}
				else if ((j == 0 && i != W - 1) || (j != W - 1 && i == 0))//beginning col
				{
					if (magArray[i][j] <= magArray[i + 1][j + 1])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else if ((j != 0 && i == W - 1) || (j == W - 1 && i != 0))
				{
					if (magArray[i][j] <= magArray[i - 1][j - 1])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				else
				{
					if (myMax(magArray[i][j], magArray[i + 1][j + 1], magArray[i - 1][j - 1]) != magArray[i][j])
					{
						magGradOut[i][j] = 0;
						removed++;
					}
					else
					{
						magGradOut[i][j] = magArray[i][j];
					}
				}
				break;
			}
			default:
				magGradOut[i][j] = 0;
				//cout << "sth wrong: " << magArray[i][j] << endl;
				break;
			}
		}
	}

	//print those pixel I got rid of using the histogram thresholding
	//cout << "number of nms point: "<<removed << " out of "<<H*W<<endl
}

void histoBuild(int array[H][W])
{
	int rowArray = H;
	int colArray = W;
	for (int i = 0; i < rowArray; i++)
	{
		for (int j = 0; j < colArray; j++)
		{
			if (array[i][j] <= 255 && array[i][j] >= 0)
			{
				countTable[array[i][j]]++;
			}
		}
	}
}

// Function for thresholding
void thresHolding(int array[H][W], bool isNaive, int threshold)
{
	int rowArray = H;
	int colArray = W;

	//naive thresholding, only take pixel with value larger than certain number
	if (isNaive == 1)
	{
		for (int i = 0; i < rowArray; i++) {
			for (int j = 0; j < colArray; j++)
			{
				if (array[i][j] <= threshold)
				{
					array[i][j] = 0;
				}
				else {
					array[i][j] = 255;
				}
			}
		}
	}
	//Here we use ptile method
	else if (isNaive == 0)
	{
		int fin_thres = 0;
		int tmp_sum = 0;
		int nzpNum = 0;

		//cout << threshold << "% " <<"number of above threshold: "<< int(threshold / 100.0*rowArray*colArray) <<"/"<<rowArray*colArray<< ' ' <<endl;
		//cout << "index 0: "<<countTable[0] << endl;

		//find which greyscale value we should take as the threshold
		//based on the percantage we provided 50% 30% 10%
		//countTable for storing histogram num
		// add up from 1 to 255 to get number of non zero pixels
		for (int i = 1; i <= 255; i++)
		{
			nzpNum += countTable[i];
		}
		for (int i = 0; i < 255; i++)
		{
			tmp_sum += countTable[255 - i];
			//cout << tmp_sum << endl;
			if (tmp_sum >= int(threshold / 100.0 * nzpNum))
			{
				fin_thres = 255 - i;
				break;
			}
		}
		//cout << "threshold: " << fin_thres << endl;
		//thresHolding(array, 1, fin_thres);

		for (int i = 0; i < rowArray; i++) {
			for (int j = 0; j < colArray; j++) {
				if (array[i][j] <= fin_thres)
				{
					array[i][j] = 0;
				}
				else {
					array[i][j] = 255;
				}
			}
		}
	}
}

// auxillary function for showing the max of these three values
int myMax(int a, int b, int c)
{
	if (a >= b)
	{
		if (a >= c)
			return a;
		else
			return c;
	}
	else
	{
		if (b >= c)
			return b;
		else
			return c;
	}
}

