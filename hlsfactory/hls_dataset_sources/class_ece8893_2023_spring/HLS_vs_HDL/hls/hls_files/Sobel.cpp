#include "Sobel.h"

void convol(int array[H * W], int sizeArray[2], int* op, int sizeOp[2], int stride, int tmpArray[H][W])
{
	int rowArray = sizeArray[0];
	int colArray = sizeArray[1];
	int rowOp = sizeOp[0];
	int colOp = sizeOp[1];


	for (int i = 0; i < rowArray; i = i + stride)
	{
		for (int j = 0; j < colArray; j = j + stride)
		{
			// use a temporary global array to transfer data
			// not a optimum way but working.
			tmpArray[i][j] = 0;
			if (i < rowOp / 2 || i >= rowArray - rowOp / 2 || j < colOp / 2 || j >= colArray - colOp / 2)
			{
				continue;
			}
			for (int p = i - rowOp / 2; p < i + rowOp / 2 + 1; p++)
			{
				for (int q = j - colOp / 2; q < j + colOp / 2 + 1; q++)
				{
					tmpArray[i][j] += array[p * colArray + q] * op[(p - (i - rowOp / 2))* colOp + (q - (j - colOp / 2))];
				}
			}
		}
	}
}

void fill(int array1[H][W], int array2[H][W], int div, int reduce)
{
	for (int i = reduce; i < H - reduce; i++)
	{
		for (int j = reduce; j < W - reduce; j++)
		{
			array2[i][j] = abs(array1[i][j]) / div;
			if (array2[i][j] > 255)
			{
				array2[i][j] = 0;
			}
		}
	}
}
