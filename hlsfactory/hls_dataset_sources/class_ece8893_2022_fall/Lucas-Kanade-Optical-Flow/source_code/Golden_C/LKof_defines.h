/*************************************************************************************
Vendor:					Xilinx 
Associated Filename:	LKof_defines.h
Purpose:				header file with all the project defines
Revision History:		31 August 2016: final release
author:					daniele.bagni@xilinx.com     
based on http://uk.mathworks.com/help/vision/ref/opticalflowlk-class.html?searchHighlight=opticalFlowLK%20class

**************************************************************************************
© Copyright 2008 - 2012 Xilinx, Inc. All rights reserved. 

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

#ifndef _H_LK_OPTICAL_FLOW_H_
#define _H_LK_OPTICAL_FLOW_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <malloc.h>

/* ******************************************************************************* */
// COMPILATION FLAGS

//#define __SDSCC__

#define ISOTROPIC_NOT_OPTIMIZED
//#define INTEGRALS_NOT_OPTIMIZED
//#define OPTIMIZED_TO_SAVE_DSP48


// to suppress some boring MS Visual C++ 2010 Express compiler warnings
#pragma warning( disable : 4996 ) // fopen, fclose, etc
#pragma warning( disable : 4068 ) // unknown HLS pragma
#pragma warning( disable : 4102 ) // unreferenced labels
#pragma warning( disable : 4715 ) // not all control paths return a value


/* ******************************************************************************* */
// CONSTANT PARAMETERS for architectural choices

#define MAX_PATH      1000
#define MAX_HEIGHT    1080  // number of lines per image
#define MAX_WIDTH     1920  // number of pixels per line


#define BITS_PER_PIXEL   12 // 8  // 12 //
#define BITS_PER_COEFF   7  // number of bits for filter coefficients

// integration 2D window size
#define SIZE_11x11

// to setup different integration window sizes and bits accumulation growth
#if defined(SIZE_3x3)
#define WINDOW_SIZE  3      
#define ACC_BITS     4
#elif defined(SIZE_5x5)
#define WINDOW_SIZE  5       
#define ACC_BITS     5
#elif defined(SIZE_7x7)
#define WINDOW_SIZE  7       
#define ACC_BITS     6
#elif defined(SIZE_11x11)
#define WINDOW_SIZE  11       
#define ACC_BITS      8
#elif defined(SIZE_15x15)
#define WINDOW_SIZE  15       
#define ACC_BITS      9
#elif defined(SIZE_25x25)
#define WINDOW_SIZE  25
#define ACC_BITS     10
#elif defined(SIZE_35x35)
#define WINDOW_SIZE  35      
#define ACC_BITS     11
#elif defined(SIZE_53x53)
#define WINDOW_SIZE  53      
#define ACC_BITS     12
#else
	#error <YOU HAVE TO DEFINE THE INTEGRATION WINDOW SIZE!>
#endif


#define FILTER_SIZE  5     // filter 2D window size
#define FILTER_OFFS   (FILTER_SIZE/2)
#define WINDOW_OFFS   (WINDOW_SIZE/2)

#define SUBPIX_BITS  3  // number of bits for subpixel accuracy (1 means 1/2, 2 means 1/4, 3 means 1/8, etc)

/* ******************************************************************************* */
// parameters depending on the above ones (do not touch)

const int hls_IMGSZ = (MAX_HEIGHT)*(MAX_WIDTH); //for HLS pragmas FIFO interfaces
const int hls_MIN_H = 480;                      //for HLS pragmas LOOP TRIPCOUNT
const int hls_MIN_W = 640;                      //for HLS pragmas LOOP TRIPCOUNT
const int hls_MAX_H = (MAX_HEIGHT);             //for HLS pragmas LOOP TRIPCOUNT
const int hls_MAX_W = (MAX_WIDTH);              //for HLS pragmas LOOP TRIPCOUNT
const int hls_WNDSZ = WINDOW_SIZE;              //for HLS pragmas LOOP TRIPCOUNT
const int HLS_STREAM_DEPTH = 10;                //for HLS pragmas STREAM
/* ******************************************************************************* */
// DATA TYPES
/*
#include "ap_int.h" // HLS arbitrary width integer data types

typedef ap_uint<BITS_PER_PIXEL  >      pix_t;      // input pixel
typedef ap_int<BITS_PER_PIXEL*2>       dualpix_t;  // to pack 2 pixels into a single larger word
typedef ap_int<    BITS_PER_PIXEL+1>   flt_t;      // for local derivatives
typedef ap_uint<3*(BITS_PER_PIXEL+1)>  p3dtyx_t;   // to pack 3 local derivatives into a single, larger word
typedef ap_int< 2*(BITS_PER_PIXEL+1)>  sqflt_t;    // for squared values of local derivatives
typedef ap_int<BITS_PER_COEFF>         coe_t;      // for filter coefficients
*/

#define  W_SUM      (2*(BITS_PER_PIXEL+1)+ACC_BITS)

/*
typedef ap_int<W_SUM>    sum_t;                 // for the accumulators of integrals computation
typedef ap_uint<5*W_SUM> p5sqflt_t;             // for 5 packed squared values of W_SUM-bit integers

typedef ap_int<2*W_SUM>    sum2_t;              // for matrix inversion
typedef ap_int<2*W_SUM+3>  det_t;               // determinant in matrix inversion
*/

typedef int sum2_t;
typedef int pix_t;

typedef float  vec_t;    // for motion vector components
typedef float  dout_t;   // for any output data
typedef float  mat_t;    // for intermediate results (debug only)

typedef struct ap_rgb {
    unsigned char  B;
    unsigned char  G;
    unsigned char  R;
  } RGB_t;

typedef enum {INTEGER, HALF, QUARTER, EIGTH} subpix_t;

typedef struct lk_vect {
   signed short int Vx;
   signed short int Vy;
} lk_vect_st;

typedef union lk_union_vect {
	int val;
	lk_vect_st packed;
} lk_union_vect_t;

/* ******************************************************************************* */
// PARAMETERS for algorithm choices
const int     THRESHOLD = (WINDOW_SIZE)*(WINDOW_SIZE);    // threshold to check if determinant is not zero
const int REF_THRESHOLD = (1.0f);                         // threshold to check if determinant is not zero

const int MAX_NUM_OF_WRONG_VECTORS = 1000; // maximum number of allowed wrong vectors

/* ******************************************************************************* */
// I/O Image Settings
#define INPUT_IMAGE1		"./test_data/car1"
#define INPUT_IMAGE2		"./test_data/car2"
#define OUTPUT_IMAGE1X		"./test_data/out_C_Vx.txt"
#define OUTPUT_IMAGE1Y		"./test_data/out_C_Vy.txt"
#define OUT_MOTCOMP			"./test_data/out_C_mcI1.txt"
#define REF_IMAGE1X			"./test_data/ref_C_Vx.txt"
#define REF_IMAGE1Y			"./test_data/ref_C_Vy.txt"
#define REF_MOTCOMP			"./test_data/ref_C_mcI1.txt"

/* ******************************************************************************* */
// SMALL FUNCTIONS IN MACRO
#define ABSDIFF(x,y)	( (x)>(y) ? (x - y) : (y - x) )
#define ABS(x)          ( (x)> 0  ? (x)     : -(x)    )
#define MIN(x,y)        ( (x)>(y) ? (y)     :  (x)    )
#define MAX(x,y)        ( (x)>(y) ? (x)     :  (y)    )


/* ******************************************************************************* */
// FUNCTION PROTOTYPES

#ifdef __SDSCC__
//#pragma SDS data mem_attribute(inp1_img:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
//#pragma SDS data mem_attribute(inp2_img:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
//#pragma SDS data mem_attribute(  vx_img:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
//#pragma SDS data mem_attribute(  vy_img:PHYSICAL_CONTIGUOUS|NON_CACHEABLE)
#pragma SDS data access_pattern(inp1_img:SEQUENTIAL)
#pragma SDS data access_pattern(inp2_img:SEQUENTIAL)
#pragma SDS data access_pattern(  vx_img:SEQUENTIAL)
#pragma SDS data access_pattern(  vy_img:SEQUENTIAL)
#pragma SDS data copy(inp1_img[0:hls_IMGSZ])
#pragma SDS data copy(inp2_img[0:hls_IMGSZ])
#pragma SDS data copy(  vx_img[0:hls_IMGSZ])
#pragma SDS data copy(  vy_img[0:hls_IMGSZ])
#pragma SDS data sys_port(inp1_img:ACP,inp2_img:ACP,vx_img:ACP,vy_img:ACP)
//#pragma SDS data data_mover(inp1_img:AXIDMA_SG,inp2_img:AXIDMA_SG,vx_img:AXIDMA_SG,vy_img:AXIDMA_SG)
#endif
/*
int hls_LK(unsigned short int inp1_img[MAX_HEIGHT*MAX_WIDTH],  unsigned short int inp2_img[MAX_HEIGHT*MAX_WIDTH],
			signed short int vx_img[WINDOW_SIZE*WINDOW_SIZE], 	signed short int vy_img[WINDOW_SIZE*WINDOW_SIZE],
		   unsigned short int height, unsigned short int width);
		   */

int ref_LK(unsigned short int *inp1_Img,  unsigned short int *inp2_Img,
		signed short int *vx_img, 	signed short int *vy_img,
		unsigned short int height, unsigned short int width);

void motion_compensation(unsigned short int *inp_img, unsigned short int *out_img, signed short int *vx_img, signed short int *vy_img, unsigned short int height, unsigned short int width, unsigned short int offset);
float compute_PSNR(unsigned short int *I1_img, unsigned short int *I2_img, unsigned int short height, unsigned short int width, unsigned short int offset);

//int hls_ComputeVectors(sum_t A11_img[WINDOW_SIZE*WINDOW_SIZE], sum_t A12_img[WINDOW_SIZE*WINDOW_SIZE],
//		               sum_t A22_img[WINDOW_SIZE*WINDOW_SIZE],  sum_t B1_img[WINDOW_SIZE*WINDOW_SIZE],
//				       sum_t B2_img[WINDOW_SIZE*WINDOW_SIZE],   int out1_img[WINDOW_SIZE*WINDOW_SIZE],
//					   unsigned short int height, unsigned short int width);
//
//bool ref_matrix_inversion(float A[2][2], float B[2], float threshold, float &Vx, float &Vy);
//unsigned char ref_isotropic_kernel(unsigned char window[FILTER_SIZE*FILTER_SIZE]);
//void ref_IsotropicFilter(unsigned short int *inp_img, unsigned char *out_img, unsigned short int height, unsigned short int width);
//void ref_HorizDerivative(unsigned char *inp_img, signed short int *out_img, unsigned short int height, unsigned short int width);
//void ref_VerticDerivative(unsigned char *inp_img, signed short int *out_img, unsigned short int height, unsigned short int width);
//void ref_TemporalDerivative(unsigned char *inp1_img, unsigned char *inp2_img, signed short int *out_img, unsigned short int height, unsigned short int width);
//int ref_ComputeVectors(float *A11_img, float *A12_img, float *A22_img, float *B1_img, float *B2_img,
//					   int *out1_img,
//					   unsigned short int height, unsigned short int width);
//void ref_ComputeIntegrals(signed short int *Ix_img, signed short int *Iy_img, signed short int *It_img,
//	                    float *A11_img, float *A12_img, float *A22_img,
//						float *B1_img, float *B2_img, unsigned short int height, unsigned short int width);
//


/* ******************************************************************************* */
// check macros

#if (BITS_PER_PIXEL > 16)
#error <MAX NUMBER OF BITS IS 16!!!!>
#endif

#endif //_H_LK_OPTICAL_FLOW_H_
