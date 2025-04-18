/*************************************************************************************
Vendor:					Xilinx
Associated Filename:	LKof_ref.cpp
Purpose:				Non Iterative Lukas Kanade Optical Flow
Revision History:		3 May 2016 - initial release
author:					daniele.bagni@xilinx.com

based on http://uk.mathworks.com/help/vision/ref/opticalflowlk-class.html?searchHighlight=opticalFlowLK%20class

**************************************************************************************
� Copyright 2008 - 2016 Xilinx, Inc. All rights reserved.

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

void ref_IsotropicFilter(unsigned short int inp_img[MAX_HEIGHT*MAX_WIDTH], unsigned char out_img[MAX_HEIGHT*MAX_WIDTH])
{
	unsigned short int row, col,tile_row, tile_col;
	unsigned char filt_out;
	signed char x,y;
	unsigned char window[FILTER_SIZE*FILTER_SIZE];

    // effective filtering (Row tiling)
	L2: for(col = FILTER_OFFS; col < 100-FILTER_OFFS; col++){
		L5:for(tile_row = 0; tile_row < FILTER_SIZE; tile_row++){
			L1: for(row = FILTER_OFFS; row<100-2*FILTER_OFFS; row+=FILTER_SIZE){

				L3:for(y=-FILTER_OFFS; y<=FILTER_OFFS; y++){
					L4:for(x=-FILTER_OFFS; x<=FILTER_OFFS; x++){
						window[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
					}
				}

				filt_out = ref_isotropic_kernel(window);
				out_img[(row+tile_row)*MAX_WIDTH+col] = filt_out;
			}
		}
	}

	L7:for(col = FILTER_OFFS; col < 100-FILTER_OFFS; col++){
		L8: for(row = 97; row<100-FILTER_OFFS; row++){
			L9:for(y=-FILTER_OFFS; y<=FILTER_OFFS; y++){
				L10:for(x=-FILTER_OFFS; x<=FILTER_OFFS; x++){
					window[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img[(y+row)*MAX_WIDTH+(x+col)];
				}
			}
			filt_out = ref_isotropic_kernel(window);
			out_img[(row)*MAX_WIDTH+col] = filt_out;
		}
	}

}

void combined_IsotropicFilter(unsigned char inp_img1[MAX_HEIGHT*MAX_WIDTH], unsigned char inp_img2[MAX_HEIGHT*MAX_WIDTH], unsigned char out_img1[MAX_HEIGHT*MAX_WIDTH], unsigned char out_img2[MAX_HEIGHT*MAX_WIDTH])
{
	unsigned short int row, col;
	unsigned char filt_out1;
	unsigned char filt_out2;
	signed char x,y;
	unsigned char window1[FILTER_SIZE*FILTER_SIZE];
	unsigned char window2[FILTER_SIZE*FILTER_SIZE];

//    // effective filtering
	L1: for(row = FILTER_OFFS; row<100-FILTER_OFFS; row++){
		L2: for(col = FILTER_OFFS; col < 100-FILTER_OFFS; col++){
			L3:for(y=-FILTER_OFFS; y<=FILTER_OFFS; y++){
				L4:for(x=-FILTER_OFFS; x<=FILTER_OFFS; x++){

					window1[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img1[(y+row)*MAX_WIDTH+(x+col)];
					window2[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img2[(y+row)*MAX_WIDTH+(x+col)];
				}
			}
			filt_out1 = ref_isotropic_kernel(window1);
			filt_out2 = ref_isotropic_kernel(window2);
			out_img1[row*100+col] = filt_out1;
			out_img2[row*100+col] = filt_out2;
		}
	}
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
 	unsigned char i, j;
// 	signed short int I3_window[WINDOW_SIZE*WINDOW_SIZE];
 	weight = (float) 0;

	for(i=0;i<WINDOW_SIZE*WINDOW_SIZE; i++){

		weight = weight + (int) I1_window[i] * (int) I2_window[i];
	}


 	return weight;
 }

 void ref_HorizDerivative(unsigned char inp_img[MAX_HEIGHT*MAX_WIDTH], signed short int out_img[MAX_HEIGHT*MAX_WIDTH])
 {
 	unsigned short int row, col;
 	signed short int filt_out;
 	signed char x,y;

 	unsigned char window[FILTER_SIZE*FILTER_SIZE];

//     effective filtering

 	L1: for(row = FILTER_OFFS; row<100-FILTER_OFFS; row++){
		L2: for(col = FILTER_OFFS; col < 100-FILTER_OFFS; col++){
			L3:for(y=-FILTER_OFFS; y<=FILTER_OFFS; y++){
				L4:for(x=-FILTER_OFFS; x<=FILTER_OFFS; x++){
						window[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img[(y+row)*MAX_WIDTH+(x+col)];
				}
			}
			filt_out = ref_Hderiv_kernel(window);
			out_img[row*100+col] = filt_out;
		}
	}

 }


 void ref_VerticDerivative(unsigned char inp_img[MAX_HEIGHT*MAX_WIDTH], signed short int out_img[MAX_HEIGHT*MAX_WIDTH])
 {
 	unsigned short int row, col;
 	signed short int filt_out;
 	signed char x,y;

 	unsigned char window[FILTER_SIZE*FILTER_SIZE];


 	L1: for(row = FILTER_OFFS; row<100-FILTER_OFFS; row++){
		L2: for(col = FILTER_OFFS; col < 100-FILTER_OFFS; col++){
			L3:for(y=-FILTER_OFFS; y<=FILTER_OFFS; y++){
				L4:for(x=-FILTER_OFFS; x<=FILTER_OFFS; x++){
						window[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img[(y+row)*MAX_WIDTH+(x+col)];
				}
			}
			filt_out = ref_Vderiv_kernel(window);
			out_img[row*100+col] = filt_out;
		}
	}

 }

 void ref_TemporalDerivative(unsigned char inp1_img[MAX_HEIGHT*MAX_WIDTH], unsigned char inp2_img[MAX_HEIGHT*MAX_WIDTH], signed short int out_img[MAX_HEIGHT*MAX_WIDTH])
 {
 	unsigned short int row, col;
// 	signed short int filt_out;

     // effective filtering
 	L1: for(row = 0; row < MAX_HEIGHT; row++)
 	{
 		L2: for(col = 0; col < MAX_WIDTH; col++)
 		{
 			out_img[(row-0)*MAX_WIDTH+(col-0)]  = inp2_img[row*MAX_WIDTH+col] - inp1_img[row*MAX_WIDTH+col];
 		}

 	}
 }


 //combined derivative


 void ref_CombinedDerivative(unsigned char inp_img[MAX_HEIGHT*MAX_WIDTH], signed short int out_img1[MAX_HEIGHT*MAX_WIDTH], signed short int out_img2[MAX_HEIGHT*MAX_WIDTH])
  {
  	unsigned short int row, col;
  	signed short int filt_out1;
  	signed short int filt_out2;
  	signed char x,y;

  	unsigned char window1[FILTER_SIZE*FILTER_SIZE];
  	unsigned char window2[FILTER_SIZE*FILTER_SIZE];
 //     effective filtering

  	L1: for(row = FILTER_OFFS; row<100-FILTER_OFFS; row++){
 		L2: for(col = FILTER_OFFS; col < 100-FILTER_OFFS; col++){

 			L3:for(y=-FILTER_OFFS; y<=FILTER_OFFS; y++){
 				L4:for(x=-FILTER_OFFS; x<=FILTER_OFFS; x++){

 					window1[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img[(y+row)*MAX_WIDTH+(x+col)];
 					window2[(y+FILTER_OFFS)*FILTER_SIZE + (x+FILTER_OFFS)] = (unsigned char) inp_img[(y+row)*MAX_WIDTH+(x+col)];

 				}
 			}
 			filt_out1 = ref_Hderiv_kernel(window1);
 			filt_out2=ref_Vderiv_kernel(window2);

 			out_img1[row*100+col] = filt_out1;
 			out_img2[row*100+col] = filt_out2;
 		}
 	}

  }


 void CombinedFilter(unsigned short int inp_img1[100*100], unsigned short int inp_img2[100*100],unsigned char out_img1[100*100], unsigned char out_img2[100*100])
 {
#pragma HLS dataflow
	    ref_IsotropicFilter(inp_img1, out_img1);
	 	ref_IsotropicFilter(inp_img2,out_img2);

 }

 void CombinedDerivative(unsigned char *inp_img1, unsigned char *inp_img2, unsigned char *inp_img3, unsigned char *inp_img4,signed short int *out_img1, signed short int *out_img2, signed short int *out_img3)
 {



	 #pragma HLS DATAFLOW
	 	ref_HorizDerivative(inp_img1, out_img1);
	     //compute vertical derivatives of Image 1
	 	ref_VerticDerivative(inp_img2, out_img2);
	 	//compute temporal derivative
	 	ref_TemporalDerivative(inp_img3, inp_img4, out_img3);

 }

 int ref_ComputeVectors(float A11_img[MAX_HEIGHT*MAX_WIDTH], float A12_img[MAX_HEIGHT*MAX_WIDTH], float A22_img[MAX_HEIGHT*MAX_WIDTH], float B1_img[MAX_HEIGHT*MAX_WIDTH], float B2_img[MAX_HEIGHT*MAX_WIDTH],
 					   signed short int vx_img[10000], signed short int vy_img[10000])
 {
 	unsigned short int row, col;
 	unsigned short int col_tile;
 	int cnt = 0;
 	lk_union_vect_t out_vect;

 	float A[2][2];
 	float B[2];

 	L1: for(row = 0; row < 100; row++)
 	{

 		L2: for(col = 0; col < 100; col+=10)
 		{

 			L3: for(col_tile = 0; col_tile < 10; col_tile++)
 			{
#pragma HLS UNROLL
 				float Vx = 0;
 				float Vy = 0;

 				A[0][0] = A11_img[(row)*MAX_WIDTH+(col + col_tile)];	//a11
 				A[0][1] = A12_img[(row)*MAX_WIDTH+(col + col_tile)];   //a12;
 				A[1][0] = A[0][1]; 	                        //a21
 				A[1][1] = A22_img[(row)*MAX_WIDTH+(col + col_tile)];   //a22;
 				B[0]    =  B1_img[(row)*MAX_WIDTH+(col + col_tile)];   //b1
 				B[1]    =  B2_img[(row)*MAX_WIDTH+(col + col_tile)];   //b2

 				bool invertible = ref_matrix_inversion(A, B, (float) THRESHOLD, Vx, Vy);

 				cnt += ((int) invertible);

 				////quantize motion vectors
 				signed short int qVx = (signed short int ) (Vx *(1<<SUBPIX_BITS));
 				signed short int qVy = (signed short int ) (Vy *(1<<SUBPIX_BITS));

 				vx_img[(row)*MAX_WIDTH+(col + col_tile)] = qVx;
 				vy_img[(row)*MAX_WIDTH+(col + col_tile)] = qVy;
 			} // end of L3



 		} // end of L2

 	} // end of L1

 	return cnt;

 	//return (int)count;

 }

 void integration_dataflow(signed short int Ix_window[WINDOW_SIZE*WINDOW_SIZE], signed short int Iy_window[WINDOW_SIZE*WINDOW_SIZE], signed short int  It_window[WINDOW_SIZE*WINDOW_SIZE], float coeff[5]){
	 signed short int Ix_window1[WINDOW_SIZE*WINDOW_SIZE];
	 signed short int Ix_window2[WINDOW_SIZE*WINDOW_SIZE];
	 signed short int Ix_window3[WINDOW_SIZE*WINDOW_SIZE];

	 signed short int Iy_window1[WINDOW_SIZE*WINDOW_SIZE];
	 signed short int Iy_window2[WINDOW_SIZE*WINDOW_SIZE];
	 signed short int Iy_window3[WINDOW_SIZE*WINDOW_SIZE];

	 signed short int It_window1[WINDOW_SIZE*WINDOW_SIZE];
	 unsigned short int row, col;
	 for(row=0; row<WINDOW_SIZE*WINDOW_SIZE; row++){
#pragma HLS PIPELINE
		 Ix_window1[row] = Ix_window[row];
		 Ix_window2[row] = Ix_window[row];
		 Ix_window3[row] = Ix_window[row];

		 Iy_window1[row] = Iy_window[row];
		 Iy_window2[row] = Iy_window[row];
		 Iy_window3[row] = Iy_window[row];

		 It_window1[row] = It_window[row];
	 }

#pragma HLS DATAFLOW
	 coeff[0]=ref_integration_kernel(Ix_window, Ix_window1);
	 coeff[1]=ref_integration_kernel(Ix_window2, Iy_window);
	 coeff[2]=ref_integration_kernel(Iy_window1, Iy_window2);
	 coeff[3]=ref_integration_kernel(Ix_window3, It_window);
	 coeff[4]=ref_integration_kernel(Iy_window3, It_window1);

 }

 void ref_ComputeIntegrals(signed short int Ix_img[MAX_HEIGHT*MAX_WIDTH], signed short int Iy_img[MAX_HEIGHT*MAX_WIDTH], signed short int It_img[MAX_HEIGHT*MAX_WIDTH],
 	                    float A11_img[MAX_HEIGHT*MAX_WIDTH], float A12_img[MAX_HEIGHT*MAX_WIDTH], float A22_img[MAX_HEIGHT*MAX_WIDTH],
 						float B1_img[MAX_HEIGHT*MAX_WIDTH], float B2_img[MAX_HEIGHT*MAX_WIDTH])
 {

 	unsigned short int row, col, tile_row;
 	signed char x,y;

 	float a11, a12, a21, a22;
 	float b1, b2;
 	float coeff[5];

 	signed short int Ix_window[WINDOW_SIZE*WINDOW_SIZE];
 	signed short int Iy_window[WINDOW_SIZE*WINDOW_SIZE];
 	signed short int It_window[WINDOW_SIZE*WINDOW_SIZE];


 	//  (Row tiling)
 	L2: for(col = WINDOW_OFFS; col < 100-WINDOW_OFFS; col++){
		L5:for(tile_row = 0; tile_row < WINDOW_SIZE; tile_row++){
			L1: for(row = WINDOW_OFFS; row<100-2*WINDOW_OFFS; row+=WINDOW_SIZE){
//				if(row+ tile_row >= 100 -WINDOW_OFFS){
//					break;
//				}

				L3:for(y=-WINDOW_OFFS; y<=WINDOW_OFFS; y++){
					L4:for(x=-WINDOW_OFFS; x<=WINDOW_OFFS; x++){
						Ix_window[(y+WINDOW_OFFS)*WINDOW_SIZE + (x+WINDOW_OFFS)] = Ix_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
						Iy_window[(y+WINDOW_OFFS)*WINDOW_SIZE + (x+WINDOW_OFFS)] = Iy_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
						It_window[(y+WINDOW_OFFS)*WINDOW_SIZE + (x+WINDOW_OFFS)] = It_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
					}
				}
//				printf("%d,%d ", row, tile_row);
				integration_dataflow(Ix_window, Iy_window, It_window, coeff);
				A11_img[(row+tile_row)*100+col] = coeff[0];
				A12_img[(row+tile_row)*100+col] = coeff[1];
				A22_img[(row+tile_row)*100+col] = coeff[2];
				 B1_img[(row+tile_row)*100+col] = coeff[3];
				 B2_img[(row+tile_row)*100+col] = coeff[4];
			}
		}
	}

	L7:for(col = WINDOW_OFFS; col < 100-WINDOW_OFFS; col++){
		L8: for(row = 100-2*WINDOW_OFFS; row<83; row++){
			L9:for(y=-WINDOW_OFFS; y<=WINDOW_OFFS; y++){
				L10:for(x=-WINDOW_OFFS; x<=WINDOW_OFFS; x++){
					Ix_window[(y+WINDOW_OFFS)*WINDOW_SIZE + (x+WINDOW_OFFS)] = Ix_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
					Iy_window[(y+WINDOW_OFFS)*WINDOW_SIZE + (x+WINDOW_OFFS)] = Iy_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
					It_window[(y+WINDOW_OFFS)*WINDOW_SIZE + (x+WINDOW_OFFS)] = It_img[(y+row+tile_row)*MAX_WIDTH+(x+col)];
				}
			}
			integration_dataflow(Ix_window, Iy_window, It_window, coeff);
			A11_img[(row)*100+col] = coeff[0];
			A12_img[(row)*100+col] = coeff[1];
			A22_img[(row)*100+col] = coeff[2];
			 B1_img[(row)*100+col] = coeff[3];
			 B2_img[(row)*100+col] = coeff[4];
		}
	}

 }



int ref_LK(unsigned short int *inp1_img,  unsigned short int *inp2_img, signed short int *vx_img, signed short int *vy_img)
{



	//--------------------------------------------------------------------------
	// Defines interface IO ports for HLS. 
	// You should NOT modify these pragmas.
	//--------------------------------------------------------------------------
	#pragma HLS INTERFACE m_axi depth=100*100  port=inp1_img   bundle=mem1
	#pragma HLS INTERFACE m_axi depth=100*100  port=inp2_img   bundle=mem2
	#pragma HLS INTERFACE m_axi depth=100*100  port=vx_img  bundle=mem1
	#pragma HLS INTERFACE m_axi depth=100*100  port=vy_img  bundle=mem2

	#pragma HLS INTERFACE s_axilite register port=return


	float A11_img[MAX_HEIGHT*MAX_WIDTH];
	float A12_img[MAX_HEIGHT*MAX_WIDTH];
	float A22_img[MAX_HEIGHT*MAX_WIDTH];
	float  B1_img[MAX_HEIGHT*MAX_WIDTH];
	float  B2_img[MAX_HEIGHT*MAX_WIDTH];
	signed short int Dx1_img[MAX_HEIGHT*MAX_WIDTH];
	signed short int Dy1_img[MAX_HEIGHT*MAX_WIDTH];
	signed short int Dt_img[MAX_HEIGHT*MAX_WIDTH];
	unsigned char flt1_img[MAX_HEIGHT*MAX_WIDTH];
	unsigned char flt2_img[MAX_HEIGHT*MAX_WIDTH];

	unsigned short int inp1_img_bram[10000], inp2_img_bram[10000];
	unsigned short int row, col;

	CombinedFilter(inp1_img, inp2_img, flt1_img, flt2_img);

////	compute horizontal derivatives of image 1
	ref_HorizDerivative(flt1_img, Dx1_img);
//////	compute vertical derivatives of Image 1
	ref_VerticDerivative(flt1_img, Dy1_img);
//	ref_CombinedDerivative(flt1_img, Dx1_img, Dy1_img);
////////	compute temporal derivativethreshold
	ref_TemporalDerivative(flt1_img, flt2_img, Dt_img);
//////
////	// compute integrals
	ref_ComputeIntegrals(Dx1_img, Dy1_img, Dt_img, A11_img, A12_img, A22_img, B1_img, B2_img);

//////	// compute vectors
	int cnt = ref_ComputeVectors(A11_img, A12_img, A22_img, B1_img, B2_img, vx_img, vy_img);
	return cnt;

}

