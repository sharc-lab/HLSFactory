/*************************************************************************************
Vendor:					Xilinx 
Associated Filename:	LKof_main.cpp
Purpose:				Testbench file for Lukas Kanade Non Iterative Optical Flow
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
#include "ap_bmp.h"
//#include "sdsoc_defines.h"

#include "motion_compensation.cpp"
#include "ap_bmp.cpp"
#include "LKof_ref.cpp"

/* **************************************************************************************** */
static unsigned char rgb2y(RGB_t pix) // routine to convert RGB into Luminance Y
{
	unsigned char y;

	// y = ((66 * pix.R + 129 * pix.G + 25 * pix.B + 128) >> 8) + 16;
	y = ((76 * pix.R + 150 * pix.G + 29 * pix.B + 128) >> 8) ;
	return y;
}
/* **************************************************************************************** */
	
// read from a text file of floating point data organized into H lines of W columns and MAX_WIDTH stride
static void Read_Txt_File(char *filename, int W, int H, float *data)
{
	FILE *fid = fopen(filename, "rt");
	if(!fid) printf("ERROR: could not open %s for reading\n",filename);
  
	unsigned short int y,x;
	float value;

	for (y = 0; y < H; y++)
	{
		for (x = 0; x < W; x++)
		{
			fscanf(fid, "%f ", &value);
			//printf( "%f ", (float) value);
			data[y*MAX_WIDTH+x] = value; 
		}
		fscanf(fid, "\n");
		//printf("\n");
	}
	fclose(fid);
}

/*
// write to a text file of floating point data organized into H lines of W columns and MAX_WIDTH stride
template <typename T> void Write_Txt_File(char *filename, int W, int H, T *data)
{
	FILE *fid = fopen(filename, "wt");

	unsigned short int y,x;
	float value;

	for (y = 0; y < H; y++)
	{
		for (x = 0; x < W; x++)
		{
			value = (float) data[y*MAX_WIDTH+x]; 
			fprintf(fid, "%20.10f ", value);
		}
		fprintf(fid, "\n");
	}
	fclose(fid);
}
*/

void Write_Txt_File(const char* filename, int W, int H, signed short int* data)
{
	FILE* fid = fopen(filename, "wt");

	unsigned short int y, x;
	float value;

	for (y = 0; y < H; y++)
	{
		for (x = 0; x < W; x++)
		{
			value = (float)data[y * MAX_WIDTH + x];
			fprintf(fid, "%20.10f ", value);
		}
		fprintf(fid, "\n");
	}
	fclose(fid);
}

void Write_Txt_File2(const char* filename, int W, int H, unsigned short int* data)
{
	FILE* fid = fopen(filename, "wt");

	unsigned short int y, x;
	float value;

	for (y = 0; y < H; y++)
	{
		for (x = 0; x < W; x++)
		{
			value = (float)data[y * MAX_WIDTH + x];
			fprintf(fid, "%20.10f ", value);
		}
		fprintf(fid, "\n");
	}
	fclose(fid);
}




/* **************************************************************************************** */
/* **************************************************************************************** */
/* **************************************************************************************** */

int main(int argc, char** argv)
{

  unsigned short int  x, y, width, height;
  char *tempbuf1, *tempbuf2;
  int check_results, ret_res=0;

  int  ref_pt, inv_points;

  // Arrays to store image data
  unsigned char *R, *G, *B;
  
  //Arrays to send and receive data from the accelerator
  unsigned short int *inp1_img;
  unsigned short int *inp2_img;

  // motion compensated image and motion vectors
  unsigned short int *mc_img, *mc_ref;
  //signed short int *vx_ref, *vy_ref, *vx_img, *vy_img;
  signed short int* vx_ref, * vy_ref;

  /* **************************************************************************************** */
   // if you want to crop the image into a smaller size here is the place to set it
   width  = 640;
   height = 480;
  
  // memory allocation
  tempbuf1 = (char *) malloc(MAX_PATH * sizeof(char));
  tempbuf2 = (char *) malloc(MAX_PATH * sizeof(char));
  R = (unsigned char *) malloc(width * height * sizeof(unsigned char));
  G = (unsigned char *) malloc(width * height * sizeof(unsigned char));
  B = (unsigned char *) malloc(width * height * sizeof(unsigned char));

  mc_img   = (unsigned short int  *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned short int));
  mc_ref   = (unsigned short int  *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned short int));
  vx_ref   = (  signed short int  *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(  signed short int));
  vy_ref   = (  signed short int  *) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(  signed short int));

  inp1_img = (unsigned short int*) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned short int));
  inp2_img = (unsigned short int*) malloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned short int));

  /*
  inp1_img = (unsigned short int  *) sds_alloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned short int));
  inp2_img = (unsigned short int  *) sds_alloc(MAX_HEIGHT * MAX_WIDTH * sizeof(unsigned short int));
    vx_img = (  signed short int  *) sds_alloc(MAX_HEIGHT * MAX_WIDTH * sizeof(  signed short int));
    vy_img = (  signed short int  *) sds_alloc(MAX_HEIGHT * MAX_WIDTH * sizeof(  signed short int));
	*/

  memset(mc_ref,  0, MAX_HEIGHT * MAX_WIDTH);
  memset(mc_img,  0, MAX_HEIGHT * MAX_WIDTH);
  //memset( vx_img, 0, MAX_HEIGHT * MAX_WIDTH);
  //memset( vy_img, 0, MAX_HEIGHT * MAX_WIDTH);
  memset( vx_ref, 0, MAX_HEIGHT * MAX_WIDTH);
  memset( vy_ref, 0, MAX_HEIGHT * MAX_WIDTH);

  /* **************************************************************************************** */
  //Get image data 1
  sprintf(tempbuf1,  "%s.bmp", INPUT_IMAGE1);
  // Fill a frame with data
  int read_tmp = BMP_Read(tempbuf1, height, width, R, G, B);
  if(read_tmp != 0) {
    printf("%s Loading image failed\n", tempbuf1);
    exit (1);
  }
  printf("Loaded image file %s of size %4d %4d\n", tempbuf1, height, width);
  // convert RGB into luminance only
   for(y = 0; y <height; y++)
   {
      for(x = 0; x <width; x++)
	  {
		  RGB_t pixel;
		  pixel.R = (unsigned char) R[y*width + x];
		  pixel.G = (unsigned char) G[y*width + x];
		  pixel.B = (unsigned char) B[y*width + x];
		  inp1_img[y*MAX_WIDTH + x] = rgb2y(pixel);
      }
   }
  //Get image data 2
  sprintf(tempbuf2,  "%s.bmp", INPUT_IMAGE2);
  // Fill a frame with data
  read_tmp = BMP_Read(tempbuf2, height, width, R, G, B);
  if(read_tmp != 0) {
    printf("%s Loading image failed\n", tempbuf2);
    exit (1);
  }
  printf("Loaded image file %s of size %4d %4d\n", tempbuf2, height, width);
  // convert RGB into luminance only
  for(y = 0; y <height; y++)
   {
      for(x = 0; x <width; x++)
	  {
		  RGB_t pixel;
		  pixel.R = (unsigned char) R[y*width + x];
		  pixel.G = (unsigned char) G[y*width + x];
		  pixel.B = (unsigned char) B[y*width + x];
		  inp2_img[y*MAX_WIDTH + x] = rgb2y(pixel);
      }
   }
   //zero padding, if needed, to emulate an image size larger than 640x480
  for(y = height; y<MAX_HEIGHT; y++)
  {
     for(x = width; x< MAX_WIDTH; x++)
     {
		  inp1_img[y*MAX_WIDTH + x] = 0;
		  inp2_img[y*MAX_WIDTH + x] = 0;
     }
  }
  /* **************************************************************************************** */

   printf("\nLukas-Kanade Single-Step Dense Optical Flow on image size of W=%4d H=%4d\n", MAX_WIDTH, MAX_HEIGHT);
   printf("integration window size of %dx%d\n", WINDOW_SIZE, WINDOW_SIZE);

   int NUM_TESTS = 1;
   if (argc >= 2)
   {
      NUM_TESTS = atoi(argv[1]);
   }
	printf("\n\nRunning ");
	for (int i=0; i<argc; i++)
		printf("%s ", argv[i]);
	printf("\n\n");

   /* **************************************************************************************** */
   /* **************************************************************************************** */

  printf("REF design\n");
  for (int i = 0; i < NUM_TESTS; i++) 
  {
	 //sw_sds_clk_start()
	 ref_pt = ref_LK(inp1_img,  inp2_img, vx_ref, vy_ref, MAX_HEIGHT, MAX_WIDTH);
	 //sw_sds_clk_stop()
  }
  printf("number of invertible points = %d, which represents %2.2f%%\n", ref_pt, (ref_pt*100.0)/(height*width));

  /*
  printf("HLS DUT\n");
  for (int i = 0; i < NUM_TESTS; i++) 
  {
	  hw_sds_clk_start()
	  inv_points = hls_LK(inp1_img,  inp2_img, vx_img, vy_img, MAX_HEIGHT, MAX_WIDTH); //height, width); // DUT: Design Under Test
	  hw_sds_clk_stop() 
  }
  printf("number of invertible points = %d, which represents %2.2f%%\n", inv_points, (inv_points*100.0)/(height*width));
  sds_print_results()
  */

  //printf("Motion Compensation of REF design\n");
  //REF motion compensation
  motion_compensation(inp2_img, mc_ref, vx_ref, vy_ref, height, width, (FILTER_SIZE+WINDOW_SIZE)/2);
  //REF compute PSNR
  float psnr = compute_PSNR(inp1_img, mc_ref, height, width, (FILTER_SIZE+WINDOW_SIZE)/2); 
  printf("PSNR of motion compensated image with REF optical flow= %4.4f dB\n", psnr);

  //printf("Motion Compensation of HLS design\n");
  //HLS motion compensation
  //motion_compensation(inp2_img, mc_img, vx_img, vy_img, height, width, (FILTER_SIZE+WINDOW_SIZE)/2);
  //HLS compute PSNR
  //psnr = compute_PSNR(inp1_img, mc_img, height, width, (FILTER_SIZE+WINDOW_SIZE)/2);
  //printf("PSNR of motion compensated image with HLS optical flow= %4.4f dB\n", psnr);
  

  ///* **************************************************************************************** */
  //// self checking test bench
  ///* **************************************************************************************** */
  /*
  printf("Checking results: REF vs. HLS\n");
  double diff1, abs_diff1, diff2, abs_diff2, total_error;
  
  total_error = 0.0f;

   // compare current vs. reference results
   check_results = 0;
   for (y = (WINDOW_OFFS+2*FILTER_OFFS);   y < height-(WINDOW_OFFS+2*FILTER_OFFS); y++)
   {
	   for (x = (WINDOW_OFFS+2*FILTER_OFFS); x < width -(WINDOW_OFFS+2*FILTER_OFFS); x++)
	   {
		   int vect1x, vect2x, vect1y, vect2y;
		   vect1x = vx_img[y*MAX_WIDTH + x];
		   vect2x = vx_ref[y*MAX_WIDTH + x];
		   vect1y = vy_img[y*MAX_WIDTH + x];
		   vect2y = vy_ref[y*MAX_WIDTH + x];
		   diff1 = vect2x - vect1x;
		   diff2 = vect2y - vect1y;
		   abs_diff1 = ABS(diff1);
		   abs_diff2 = ABS(diff2);
		   if (abs_diff1 > 1)
		   {
			   total_error += abs_diff1;
			   printf("Vx: expected %20.10f got %20.10f\n", (float) vect2x, (float) vect1x);
			   check_results++;
		   }
		   if (abs_diff2 > 1)
		   {
			   total_error += abs_diff2;
			   printf("Vy: expected %20.10f got %20.10f\n", (float) vect2y, (float) vect1y);
			   check_results++;
		   }
	   }
   }
   */
   printf("Test done\n");
   /*
   if (check_results > MAX_NUM_OF_WRONG_VECTORS)
   {
     printf("TEST FAILED!: error = %d\n", check_results);
     ret_res = 1;
   }
   else
   {
     printf("TEST SUCCESSFUL!\n");
 	ret_res = 0;
   }
   */

   /* **************************************************************************************** */
   // write files
   //Write_Txt_File<signed short int>(OUTPUT_IMAGE1X, width, height, vx_img); // output    Vx motion vectors file
   //Write_Txt_File<signed short int>(   REF_IMAGE1X, width, height, vx_ref); // reference Vx motion vectors file
   Write_Txt_File(REF_IMAGE1X, width, height, vx_ref); // reference Vx motion vectors file
   //Write_Txt_File<signed short int>(OUTPUT_IMAGE1Y, width, height, vy_img); // output    Vy motion vectors file
   //Write_Txt_File<signed short int>(   REF_IMAGE1Y, width, height, vy_ref); // reference Vy motion vectors file
   Write_Txt_File(REF_IMAGE1Y, width, height, vy_ref); // reference Vy motion vectors file

   //Write_Txt_File<unsigned short int>(OUT_MOTCOMP, width, height, mc_img); // output    motion compensated file
   Write_Txt_File2(REF_MOTCOMP, width, height, mc_ref); // reference motion compensated file

  /* **************************************************************************************** */
  // free memory
  free(R); free(G); free(B);
  free(tempbuf1);  free(tempbuf2);
  //free(mc_img); free(vx_ref); free(vy_ref); free(mc_ref);
  //sds_free(inp1_img); sds_free(inp2_img); sds_free(vx_img); sds_free(vy_img);
  free(vx_ref); 
  free(vy_ref); 
  free(mc_ref);
  free(inp1_img); 
  free(inp2_img);

  printf("LK OF Over\n");

  return ret_res;

}

