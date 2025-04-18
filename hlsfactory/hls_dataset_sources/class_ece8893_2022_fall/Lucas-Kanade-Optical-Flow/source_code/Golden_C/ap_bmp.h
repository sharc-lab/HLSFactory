/*******************************************************************************
Vendor: Xilinx 
Associated Filename: ap_bmp.h
Purpose: BMP image reader and writer header file for AutoESL  
Revision History: February 13, 2012 - initial release
                                                
*******************************************************************************
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

#ifndef __XLNX__BITMAP__
#define __XLNX__BITMAP__

// to suppress MS Visual C++ 2-10 Express compiler warnings
#pragma warning( disable : 4996 ) // fopen, fclose, etc
#pragma warning( disable : 4068 ) // unknown HLS pragma
#pragma warning( disable : 4102 ) // unreferenced labels
#pragma warning( disable : 4715 ) // not all control paths return a value

// Basic color definitions
#define BLACK 0
#define WHITE 255

// Maximum image size 
#define MAX_ROWS 1080
#define MAX_COLS 1920

//File Information Header
typedef struct{
  unsigned short FileType;
  unsigned int FileSize;
  unsigned short Reserved1;
  unsigned short Reserved2;
  unsigned short Offset;
}BMPHeader;

typedef struct{
  unsigned int Size;
  unsigned int Width;
  unsigned int Height;
  unsigned short Planes;
  unsigned short BitsPerPixel;
  unsigned int Compression;
  unsigned int SizeOfBitmap;
  unsigned int HorzResolution;
  unsigned int VertResolution;
  unsigned int ColorsUsed;
  unsigned int ColorsImportant;
}BMPImageHeader;

typedef struct{
  BMPHeader *file_header;
  BMPImageHeader *image_header;
  unsigned int *colors;
  unsigned char *data;
  unsigned char R[MAX_ROWS][MAX_COLS];
  unsigned char G[MAX_ROWS][MAX_COLS];
  unsigned char B[MAX_ROWS][MAX_COLS];
  unsigned char Y[MAX_ROWS][MAX_COLS];
  char U[MAX_ROWS][MAX_COLS];
  char V[MAX_ROWS][MAX_COLS];
}BMPImage;

//Read Function
int BMP_Read(char *file, int row, int col, unsigned char *R, unsigned char *G, unsigned char *B);

//Write Function
int BMP_Write(char *file, int row, int col, unsigned char *R, unsigned char *G, unsigned char *B);

#endif
