#include "gausFilter.h"

void gausFilter(int INPUT_DRAM[H][W])
{

#pragma HLS interface m_axi depth=1 port=INPUT_DRAM offset=slave bundle=mem

#pragma HLS interface s_axilite port=return

	int input[H][W];
#pragma HLS ARRAY_PARTITION variable=input dim=1 type=cyclic factor= 10
#pragma HLS ARRAY_PARTITION variable=input dim=2 type=cyclic factor= 10
	int oneDImgArray[H * W] = { 0 };
	int tmpConvArray[H][W] = { 0 };
	int givenGausFil[49] = { 1,1,2,2,2,1,1, 1,2,2,4,2,2,1, 2,2,4,8,4,2,2 ,2,4,8,16,8,4,2 , 2,2,4,8,4,2,2 , 1,2,2,4,2,2,1 , 1,1,2,2,2,1,1 };

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			input[i][j] = INPUT_DRAM[i][j];
		}
	}


#pragma HLS ARRAY_PARTITION variable=oneDImgArray type=cyclic factor=2
	int sizeImg[2] = { H,W };
	int sizeOp[2] = { GAUS_SIZE,GAUS_SIZE };
	std::cout << "start gaussian filtering:" << std::endl;

	//since my convolution fucntion code was done in a 1d array
	// transform 2d to 1 d
	for (int i = 0; i < H; i++)
	{
		for (int j = 0; j < W; j++)
		{
			oneDImgArray[i * W + j] = input[i][j];
		}
	}


	convol(oneDImgArray, sizeImg, givenGausFil, sizeOp, 1, tmpConvArray);
	fill(tmpConvArray, input, 140, 3);

	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			INPUT_DRAM[i][j] = input[i][j];
		}
	}
}
