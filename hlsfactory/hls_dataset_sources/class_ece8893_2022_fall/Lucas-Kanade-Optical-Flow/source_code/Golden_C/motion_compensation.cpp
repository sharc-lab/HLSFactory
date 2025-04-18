/*************************************************************************************
Vendor:					Xilinx 
Associated Filename:	motion_compensation.cpp
Purpose:				Luminance motion compensation with subpixel accuracy
Revision History:		3 May 2016 - initial release
author:					daniele.bagni@xilinx.com        


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


static pix_t bilinear_interpolation(pix_t A, pix_t B, pix_t C, pix_t D, float k_x, float k_y)
{
	float P1, P2, P3;
	pix_t out_pix;

	//first linear interpolation: horizontal direction
	P1 = (1-k_x)*A + k_x*B;
	//second linear interpolation: horizontal direction
	P2 = (1-k_x)*C + k_x*D;
	//third linear interpolation: vertical direction
	P3 = (1-k_y)*P1 + k_y*P2;

	out_pix = (pix_t) P3;
	return out_pix;
}


void motion_compensation( unsigned short int *inp_img, unsigned short int *out_img, signed short int *vx_img, signed short int *vy_img,
		                  unsigned short int height, unsigned short int width, unsigned short int offset)
{
	unsigned short int row, col;

	float tot_x, tot_y;   //total coordinates
	short int ix0, iy0, ix1, iy1;  // integer coordinates

	float k_x, k_y, Vx, Vy;

	signed short int vect_x, vect_y;

	pix_t A, B, C, D, P3;

	L1: for(row = 0; row < height; row++)
	{

		L2: for(col = 0; col < width; col++)
		{
	
			if ( (row < offset)  |  (col < offset) | (row > (height-offset)) |  (col > (width-offset)) )  //do nothing
			{
					out_img[(row)*MAX_WIDTH+(col)] = (unsigned char) inp_img[(row)*MAX_WIDTH+(col)];
			}
			else
			{
				vect_x = vx_img[(row)*MAX_WIDTH+(col)];
				vect_y = vy_img[(row)*MAX_WIDTH+(col)];

				Vx = ((float) vect_x) / (1<<SUBPIX_BITS);
				Vy = ((float) vect_y) / (1<<SUBPIX_BITS);
				
				// total coordinates with fractional values
				tot_x = col + Vx;
				tot_y = row + Vy;

				// get integer coordinates
				ix0 = ((1<<SUBPIX_BITS) * tot_x); 	//subpixel accuracy is 1/8
				iy0 = ((1<<SUBPIX_BITS) * tot_y);
				ix0 = ix0/(1<<SUBPIX_BITS);
				iy0 = iy0/(1<<SUBPIX_BITS);
				ix1 = ix0+1;
				iy1 = iy0+1;

				//fractional residuals (they must have values between 0 and 1)
				k_x = ABS(tot_x) - ABS(ix0);
				k_y = ABS(tot_y) - ABS(iy0);
				if ( (k_x<0) | (k_y<0) |  (k_x>1) | (k_y>1) )
				{
					//assert(k_x>0); assert(k_x<1); 
					//assert(k_y>0); assert(k_y<1);
					printf("Vx=%4.4f Vy=%4.4f col=%4d row=%4d tot_x=%4.4f tot_y=%4.4f ix0=%4d iy0=%4d kx=%4.4f ky=%4.4f\n", 
						Vx, Vy, col, row, tot_x, tot_y, ix0, iy0, k_x, k_y); //DB: controlalre qui
				}

				if ( (iy0>0) & (ix0>0) & (iy1<MAX_HEIGHT) & (ix1<MAX_WIDTH) )
				{
					//bilinear interpolation
					A = (pix_t) inp_img[iy0*MAX_WIDTH+ix0];
					B = (pix_t) inp_img[iy0*MAX_WIDTH+ix1];
					C = (pix_t) inp_img[iy1*MAX_WIDTH+ix0];
					D = (pix_t) inp_img[iy1*MAX_WIDTH+ix1];
					P3 = bilinear_interpolation(A, B, C, D, k_x, k_y);
				}
				else
					P3 = 0;

				out_img[(row)*MAX_WIDTH+(col)] = (unsigned char) P3;

			}

		} // end of L2

	} // end of L1


}



float compute_PSNR(unsigned short int *I1_img, unsigned short int *I2_img, unsigned short int height, unsigned short int width, unsigned short int offset)
{
	unsigned short int row, col;

	float MSE, tot_diff, peak2, psnr;
	int diff, abs_diff, diff2;

	tot_diff = 0;
	L1: for(row = offset-1; row < height-offset; row++)
	{

		L2: for(col = offset-1; col < width-offset; col++)
		{   
			diff  = I1_img[(row)*MAX_WIDTH+(col)] - I2_img[(row)*MAX_WIDTH+(col)];
			abs_diff = ABS(diff);
		    diff2 = abs_diff*abs_diff;
			tot_diff +=diff2;
		}
	}

	MSE = tot_diff / ((height-2*offset)*(width-2*offset));

	peak2 = 255*255;
	psnr = 10*log10(peak2/MSE);

	return psnr;

}

