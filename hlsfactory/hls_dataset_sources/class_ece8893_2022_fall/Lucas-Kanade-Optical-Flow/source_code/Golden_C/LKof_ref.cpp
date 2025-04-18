/*************************************************************************************
Vendor:					Xilinx 
Associated Filename:	LKof_ref.cpp
Purpose:				Non Iterative Lukas Kanade Optical Flow
Revision History:		3 May 2016 - initial release
author:					daniele.bagni@xilinx.com        

based on http://uk.mathworks.com/help/vision/ref/opticalflowlk-class.html?searchHighlight=opticalFlowLK%20class

**************************************************************************************
© Copyright 2008 - 2016 Xilinx, Inc. All rights reserved. 

This file contains confidential and proprietary information of Xilinx, Inc. and 
is protected under U.S. and international copyright and other intellectual 
property laws.

DISCLAIMER
This disclaimer is not a license and does not grant any rights to the materials 
distributed herewith. Except as otherwise provided in a valid license issued to 
you by Xilinx, and to the maximum extent permitted by applicable law: 
(1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL FAULTS, AND XILINX 
HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS, IMPLIED, OR STATUTORY, 
INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT, OR 
FITNESS FOR ANY PARTICULAR PURPOSE; and (2) Xilinx shall not be liable (whether 
in contract or tort, including negligence, or under any other theory of 
liability) for any loss or damage of any kind or nature related to, arising under 
or in connection with these materials, including for any direct, or any indirect, 
special, incidental, or consequential loss or damage (including loss of data, 
profits, goodwill, or any type of loss or damage suffered as a result of any 
action brought by a third party) even if such damage or loss was reasonably 
foreseeable or Xilinx had been advised of the possibility of the same.

CRITICAL APPLICATIONS
Xilinx products are not designed or intended to be fail-safe, or for use in any 
application requiring fail-safe performance, such as life-support or safety 
devices or systems, Class III medical devices, nuclear facilities, applications 
related to the deployment of airbags, or any other applications that could lead 
to death, personal injury, or severe property or environmental damage 
(individually and collectively, "Critical Applications"). Customer assumes the 
sole risk and liability of any use of Xilinx products in Critical Applications, 
subject only to applicable laws and regulations governing limitations on product 
liability. 

THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE AT 
ALL TIMES.

*******************************************************************************/

#include "LKof_defines.h"



bool ref_matrix_inversion(float A[2][2], float B[2], float threshold, float &Vx, float &Vy)
{
	bool invertible = 0;
	float inv_A[2][2];
	float a, b, c, d; 
	float det_A, abs_det_A, neg_det_A, zero = 0;
	float recipr_det_A;

	a = A[0][0]; b = A[0][1]; c = A[1][0]; d = A[1][1];

	float a_x_d, b_x_c, mult1, mult2, mult3, mult4;
	float t_Vx, t_Vy;

	//det_A = (a*d)-(b*c); //determinant of matrix  A = [a b; c d]
	a_x_d =  a *  d;
	b_x_c =  b *  c;
	det_A = a_x_d - b_x_c;
	neg_det_A = (zero-det_A);
	abs_det_A = (det_A > zero) ? det_A : neg_det_A;

	recipr_det_A = (1.0f)/det_A;

	//compute the inverse of matrix A anyway even if it is not invertible: inv_A = [d -b; -c a]/det_A
	// note that 1/det_A is done only at the last instruction to save resources
	if (det_A == 0) recipr_det_A = 0;
	inv_A[0][0] =  d;
	inv_A[0][1] = -b;
	inv_A[1][0] = -c;
	inv_A[1][1] =  a;

	//solve the matrix equation: [Vx Vy] = -inv_A[] * B[]
	//Vx = -(inv_A[0][0] * B[0] + inv_A[0][1] * B[1]);
	//Vy = -(inv_A[1][0] * B[0] + inv_A[1][1] * B[1]);
	mult1 = (sum2_t) inv_A[0][0] * (sum2_t) B[0];
	mult2 = (sum2_t) inv_A[0][1] * (sum2_t) B[1];
	mult3 = mult2; //(sum2_t) inv_A[1][0] * (sum2_t) B[0];
	mult4 = (sum2_t) inv_A[1][1] * (sum2_t) B[1];
	t_Vx = -(mult1 + mult2);
	t_Vy = -(mult3 + mult4);

	Vx = t_Vx * recipr_det_A;
	Vy = t_Vy * recipr_det_A;

	if (det_A == 0) // zero input pixels
	{
		invertible = 0;
		Vx = 0; Vy = 0;
	}
	else if (abs_det_A < threshold) // the matrix is not invertible
	{
		invertible = 0;
		Vx = 0; Vy = 0;
	}
	else
	{
		invertible = 1;
	}

	return invertible;

}

unsigned char ref_isotropic_kernel(unsigned char window[FILTER_SIZE*FILTER_SIZE])
{
	
	// isotropic smoothing filter 5x5
	const signed short int coeff[FILTER_SIZE][FILTER_SIZE] = {
		{ 1,    4,    6,    4,    1},
		{ 4,   16,   24,   16,    4},
		{ 6,   24,   36,   24,    6},
		{ 4,   16,   24,   16,    4},
		{ 1,    4,    6,    4,    1}
	};

	// local variables
	int accum = 0;
	int normalized_accum;
	unsigned char final_val;
	unsigned char i, j;

	//Compute the 2D convolution
	for (i = 0; i < FILTER_SIZE; i++) {
		for (j = 0; j < FILTER_SIZE; j++) {

			accum = accum + ((short int) window[i*FILTER_SIZE+j] * (short int) coeff[i][j]);
		}
	}

	// do the correct normalization if needed
	normalized_accum = accum / 256;

	final_val = (unsigned char) normalized_accum;

	return final_val;
}

void ref_IsotropicFilter(unsigned short int *inp_img, unsigned char *out_img, unsigned short int height, unsigned short int width)
{
	unsigned short int row, col;
	unsigned char filt_out;
	signed char x,y;

	unsigned char window[FILTER_SIZE*FILTER_SIZE];

   // effective filtering
	L1: for(row = 0; row < height+FILTER_OFFS; row++)
	{

		L2: for(col = 0; col < width+FILTER_OFFS; col++)
		{

			//This design assumes there are no edges on the boundary of the image
			if ( (row >= FILTER_SIZE-1) & (col >= FILTER_SIZE-1) &  (row < height)  & (col< width) )
			{
				   L3:for (y=-FILTER_SIZE+1; y<=0; y++)
				   {
					   L4:for (x=-FILTER_SIZE+1; x<=0; x++)
					   {
						   window[(FILTER_SIZE-1+y)*FILTER_SIZE+(FILTER_SIZE-1+x)] = (unsigned char) inp_img[(row+y)*MAX_WIDTH+col+x];
					   }
				   }
				   filt_out = ref_isotropic_kernel(window);
			}
			else
				filt_out = 0;

			if ( (row >= FILTER_OFFS) & (col >= FILTER_OFFS) )
			{
				   out_img[(row-FILTER_OFFS)*MAX_WIDTH+(col-FILTER_OFFS)] = (filt_out);
			}

		} // end of L2
	} // end of L1

}


static signed short int ref_Hderiv_kernel(unsigned char window[FILTER_SIZE*FILTER_SIZE])
{
	
	// derivative filter in a 5x5 kernel size: [-1 8 0 -8 1]
	// coefficients are swapped to get same results as MATLAB
	const signed short int coeff[FILTER_SIZE][FILTER_SIZE] = {
		{ 0,    0,    0,    0,    0},
		{ 0,    0,    0,    0,    0},
		{ 1,   -8,    0,    8,   -1},
		{ 0,    0,    0,    0,    0},
		{ 0,    0,    0,    0,    0}
	};

	// local variables
	int accum = 0;
	int normalized_accum;
	signed short int final_val;
	unsigned char i, j;

	//Compute the 2D convolution
	for (i = 0; i < FILTER_SIZE; i++) {
		for (j = 0; j < FILTER_SIZE; j++) {

			accum = accum + ((short int) window[i*FILTER_SIZE+j] * (short int) coeff[i][j]);
		}
	}

	// do the correct normalization if needed
	normalized_accum = accum / 12;

	final_val = (signed short int) normalized_accum;

	return final_val;
}

static signed short int ref_Vderiv_kernel(unsigned char window[FILTER_SIZE*FILTER_SIZE])
{
	
	// derivative filter in a 5x5 kernel size  [-1 8 0 -8 1]^T
	// coefficients are swapped to get same results as MATLAB
	const signed short int coeff[FILTER_SIZE][FILTER_SIZE] = {
		{ 0,    0,    1,    0,    0},
		{ 0,    0,   -8,    0,    0},
		{ 0,    0,    0,    0,    0},
		{ 0,    0,    8,    0,    0},
		{ 0,    0,   -1,    0,    0}
	};

	// local variables
	int accum = 0;
	int normalized_accum;
	signed short int final_val;
	unsigned char i, j;

	//Compute the 2D convolution
	for (i = 0; i < FILTER_SIZE; i++) {
		for (j = 0; j < FILTER_SIZE; j++) {

			accum = accum + ((short int) window[i*FILTER_SIZE+j] * (short int) coeff[i][j]);
		}
	}

	// do the correct normalization if needed
	normalized_accum = accum / 12;

	final_val = (signed short int) normalized_accum;

	return final_val;
}

static float ref_integration_kernel(signed short int I1_window[WINDOW_SIZE*WINDOW_SIZE], signed short int I2_window[WINDOW_SIZE*WINDOW_SIZE])
{
	// local variables
	float weight = (float) 0;
	int squared_val, mult;
	float final_val;
	unsigned char i, j;

	//Compute the 2D convolution
	for (i = 0; i < WINDOW_SIZE; i++) {
		for (j = 0; j < WINDOW_SIZE; j++) {

			squared_val = (int) I1_window[i*WINDOW_SIZE+j] * (int) I2_window[i*WINDOW_SIZE+j];
			mult = (squared_val);
			weight = weight + mult;
		}
	}

	final_val = weight;

	return final_val;
}

void ref_HorizDerivative(unsigned char *inp_img, signed short int *out_img, unsigned short int height, unsigned short int width) 
{
	unsigned short int row, col;
	signed short int filt_out;
	signed char x,y;

	unsigned char window[FILTER_SIZE*FILTER_SIZE];

   // effective filtering
	L1: for(row = 0; row < height+FILTER_OFFS; row++)
	{
		L2: for(col = 0; col < width+FILTER_OFFS; col++)
		{
			//This design assumes there are no edges on the boundary of the image
			if ( (row >= FILTER_SIZE-1) & (col >= FILTER_SIZE-1) &  (row < height)  & (col< width) )
			{
				   L3:for (y=-FILTER_SIZE+1; y<=0; y++)
				   {
					   L4:for (x=-FILTER_SIZE+1; x<=0; x++)
					   {
						   window[(FILTER_SIZE-1+y)*FILTER_SIZE+(FILTER_SIZE-1+x)] = inp_img[(row+y)*MAX_WIDTH+col+x];
					   }
				   }
				   filt_out = ref_Hderiv_kernel(window);
			}
			else
				filt_out = 0;

			if ( (row >= FILTER_OFFS) & (col >= FILTER_OFFS) )
			{
				   out_img[(row-FILTER_OFFS)*MAX_WIDTH+(col-FILTER_OFFS)] = (filt_out);
			}


		} // end of L2
	} // end of L1


}


void ref_VerticDerivative(unsigned char *inp_img, signed short int *out_img, unsigned short int height, unsigned short int width) 
{
	unsigned short int row, col;
	signed short int filt_out;
	signed char x,y;

	unsigned char window[FILTER_SIZE*FILTER_SIZE];

   // effective filtering
	L1: for(row = 0; row < height+FILTER_OFFS; row++)
	{
		L2: for(col = 0; col < width+FILTER_OFFS; col++)
		{

			if ( (row < height)  & (col< width) )
				out_img[(row)*MAX_WIDTH+(col)] = 0;

			//This design assumes there are no edges on the boundary of the image
			if ( (row >= FILTER_SIZE-1) & (col >= FILTER_SIZE-1) &  (row < height)  & (col< width) )
			{
				   L3:for (y=-FILTER_SIZE+1; y<=0; y++)
				   {
					   L4:for (x=-FILTER_SIZE+1; x<=0; x++)
					   {
						   window[(FILTER_SIZE-1+y)*FILTER_SIZE+(FILTER_SIZE-1+x)] = inp_img[(row+y)*MAX_WIDTH+col+x];
					   }
				   }
				   filt_out = ref_Vderiv_kernel(window);
			}
			else
				filt_out = 0;

			if ( (row >= FILTER_OFFS) & (col >= FILTER_OFFS) )
			{
				   out_img[(row-FILTER_OFFS)*MAX_WIDTH+(col-FILTER_OFFS)] = (filt_out);
			}

		} // end of L2
	} // end of L1


}

void ref_TemporalDerivative(unsigned char *inp1_img, unsigned char *inp2_img, signed short int *out_img, unsigned short int height, unsigned short int width) 
{
	unsigned short int row, col;
	signed short int filt_out;
	
    // effective filtering
	L1: for(row = 0; row < height; row++)
	{
		L2: for(col = 0; col < width; col++)
		{
			filt_out = inp2_img[row*MAX_WIDTH+col] - inp1_img[row*MAX_WIDTH+col];
			out_img[(row-0)*MAX_WIDTH+(col-0)] = (filt_out);
		}

		} // end of L2
	} // end of L1



int ref_ComputeVectors(float *A11_img, float *A12_img, float *A22_img, float *B1_img, float *B2_img, 
					   signed short int *vx_img, signed short int *vy_img,
					   unsigned short int height, unsigned short int width)
{
	unsigned short int row, col;
	int cnt = 0;

	float Vx, Vy;
	signed short int qVx, qVy;
	lk_union_vect_t out_vect;

	float A[2][2]; 
	float B[2];
	//const float norm_factor = (1.0f)/ (WINDOW_SIZE*WINDOW_SIZE); //normalization factor

	L1: for(row = 0; row < height; row++)
	{

		L2: for(col = 0; col < width; col++)
		{
				Vx = 0;
				Vy = 0;

				A[0][0] = A11_img[(row)*MAX_WIDTH+(col)];	//a11
				A[0][1] = A12_img[(row)*MAX_WIDTH+(col)];   //a12;
				A[1][0] = A[0][1]; 	                        //a21
				A[1][1] = A22_img[(row)*MAX_WIDTH+(col)];   //a22;
				B[0]    =  B1_img[(row)*MAX_WIDTH+(col)];   //b1
				B[1]    =  B2_img[(row)*MAX_WIDTH+(col)];   //b2

				bool invertible = ref_matrix_inversion(A, B, (float) THRESHOLD, Vx, Vy);
				cnt = cnt + ((int) invertible); //number of invertible points found

				////quantize motion vectors
				qVx = (signed short int ) (Vx *(1<<SUBPIX_BITS));
				qVy = (signed short int ) (Vy *(1<<SUBPIX_BITS));
				assert( (qVx < 32768) &  (qVx > -32768) );
				assert( (qVy < 32768) &  (qVy > -32768) );

				vx_img[(row)*MAX_WIDTH+(col)] = qVx;
				vy_img[(row)*MAX_WIDTH+(col)] = qVy;

		} // end of L2

	} // end of L1

	return cnt;

}

void ref_ComputeIntegrals(signed short int *Ix_img, signed short int *Iy_img, signed short int *It_img, 
	                    float *A11_img, float *A12_img, float *A22_img, 
						float *B1_img, float *B2_img, unsigned short int height, unsigned short int width) 
{

	unsigned short int row, col;
	signed char x,y;

	float a11, a12, a21, a22;
	float b1, b2;

	signed short int Ix_window[WINDOW_SIZE*WINDOW_SIZE];
	signed short int Iy_window[WINDOW_SIZE*WINDOW_SIZE];
	signed short int It_window[WINDOW_SIZE*WINDOW_SIZE];

	L1: for(row = 0; row < height+WINDOW_OFFS; row++)
	{
		L2: for(col = 0; col < width+WINDOW_OFFS; col++)
		{

			//This design assumes there are no edges on the boundary of the image
			if ( (row >= WINDOW_SIZE-1) & (col >= WINDOW_SIZE-1) &  (row < height)  & (col< width) )
			{
				L3:for (y=-WINDOW_SIZE+1; y<=0; y++)
				{
					L4:for (x=-WINDOW_SIZE+1; x<=0; x++)
					{
						 Ix_window[(WINDOW_SIZE-1+y)*WINDOW_SIZE+(WINDOW_SIZE-1+x)] = Ix_img[(row+y)*MAX_WIDTH+col+x];
						 Iy_window[(WINDOW_SIZE-1+y)*WINDOW_SIZE+(WINDOW_SIZE-1+x)] = Iy_img[(row+y)*MAX_WIDTH+col+x];
						 It_window[(WINDOW_SIZE-1+y)*WINDOW_SIZE+(WINDOW_SIZE-1+x)] = It_img[(row+y)*MAX_WIDTH+col+x];
					}
				 }

				//Compute the 2D integration 
				a11=ref_integration_kernel(Ix_window, Ix_window);
				a12=ref_integration_kernel(Ix_window, Iy_window);
				a22=ref_integration_kernel(Iy_window, Iy_window);
				 b1=ref_integration_kernel(Ix_window, It_window);
				 b2=ref_integration_kernel(Iy_window, It_window);
			}
			else
			{
				a11=0; a22=0; a21=0; a12=0; b1=0; b2=0;
			}

			if ( (row >= WINDOW_OFFS) & (col >= WINDOW_OFFS) ) //&  (row < height)  & (col< width) )
			{
				 //output data are not normalized
				A11_img[(row-WINDOW_OFFS)*MAX_WIDTH+(col-WINDOW_OFFS)] = a11;
				A12_img[(row-WINDOW_OFFS)*MAX_WIDTH+(col-WINDOW_OFFS)] = a12;
				A22_img[(row-WINDOW_OFFS)*MAX_WIDTH+(col-WINDOW_OFFS)] = a22;
				 B1_img[(row-WINDOW_OFFS)*MAX_WIDTH+(col-WINDOW_OFFS)]  = b1;
				 B2_img[(row-WINDOW_OFFS)*MAX_WIDTH+(col-WINDOW_OFFS)]  = b2;
			} // end of if 
	
		} // end of L2
	} // end of L1

}



int ref_LK(unsigned short int *inp1_img,  unsigned short int *inp2_img, signed short int *vx_img, signed short int *vy_img,
	unsigned short int height, unsigned short int width)
{

	float *A11_img = (float *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(float));
	float *A12_img = (float *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(float));
	float *A22_img = (float *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(float));
	float  *B1_img = (float *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(float));
	float  *B2_img = (float *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(float));
	signed short int *Dx1_img = (signed short int *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(signed short int));
	signed short int *Dy1_img = (signed short int *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(signed short int));
	signed short int  *Dt_img = (signed short int *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(signed short int));
	unsigned char *flt1_img= (unsigned char *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned char));
	unsigned char *flt2_img= (unsigned char *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned char));

	memset(flt1_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(flt2_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(Dx1_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(Dy1_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(Dt_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(A11_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(A12_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(A22_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(B1_img, 0, MAX_HEIGHT*MAX_WIDTH);
	memset(B2_img, 0, MAX_HEIGHT*MAX_WIDTH);

	// smooth both images with same 2D filter kernel
	ref_IsotropicFilter(inp1_img, flt1_img, height, width);
	ref_IsotropicFilter(inp2_img, flt2_img, height, width);

	//compute horizontal derivatives of image 1
	ref_HorizDerivative(flt1_img, Dx1_img, height, width);
	//compute vertical derivatives of Image 1
	ref_VerticDerivative(flt1_img, Dy1_img, height, width);	
	//compute temporal derivative
	ref_TemporalDerivative(flt1_img, flt2_img, Dt_img, height, width);

	// compute integrals
	ref_ComputeIntegrals(Dx1_img, Dy1_img, Dt_img, A11_img, A12_img, A22_img, B1_img, B2_img, height, width);

	// compute vectors
	int cnt = ref_ComputeVectors(A11_img, A12_img, A22_img, B1_img, B2_img, vx_img, vy_img, height, width);


	free(A11_img);
	free(A12_img);
	free(A22_img);
	free(B1_img);
	free(B2_img);
	free(Dx1_img);
	free(Dy1_img);
	free(Dt_img);
	free(flt1_img);
	free(flt2_img);

	return cnt;

}
