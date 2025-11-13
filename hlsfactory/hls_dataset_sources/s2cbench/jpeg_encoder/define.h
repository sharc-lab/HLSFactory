//========================================================================================
// 
// File Name    : define.h
// Description  : Main definition header
// Release Date : 30/5/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author         Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0   UTD DARClab     JPEG encoder main definition header            
//=======================================================================================
#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"
#include "stdio.h"


#define BYTES_PER_PIXEL 1
#define COLS 512 * BYTES_PER_PIXEL
#define ROWS 512 * BYTES_PER_PIXEL
#define SIZE ROWS* COLS
#define IMAGE_IN "lena512.bmp"
#define IMAGE_OUT "lena512_out.jpg" 
#define IMAGE_GOLDEN "lena512_golden.jpg"

 
#define DIFFFILENAME "jpeg_diff.txt"

#define SIZE_BUFFER 8

typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef char BYTE;

#pragma pack(push,1)
typedef struct {
  WORD  bfType;                       // The type of the image
 
  DWORD bfSize;                       //size of the file
  WORD  bfReserved;                 // reserved type
  WORD  bfReserved2;               
  DWORD bfOffBits;                   //offset bytes from the file header to the image data
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct tagBITMAPINFOHEADER {
  DWORD biSize;                   //the size of the header
  LONG  biWidth;                 //the width in pixels
  LONG  biHeight;                //the height in pixels
  WORD  biPlanes;                //the no. of planes in the bitmap
  WORD  biBitCount;              //bits per pixel
  DWORD biCompression;           //compression specifications
  DWORD biSizeImage;            //size of the bitmap data
  LONG  biXPelsPerMeter;        //horizontal res(pixels per meter)
  LONG  biYPelsPerMeter;        //vertical res(pixels per meter) 
  DWORD biClrUsed;              //colours used in the image
  DWORD biClrImportant;        //num of important colours used
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

#define DCT_OUT_WIDTH 12   //DCT output bitwidth
#define QUAN_OUT_WIDTH 10  //quantization output bitwidth
#define RLE_BUFF_LEN 128   //RLE output buffer length


#endif
