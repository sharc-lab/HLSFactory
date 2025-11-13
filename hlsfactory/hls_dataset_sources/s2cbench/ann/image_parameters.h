//========================================================================================
// 
// File Name    : image_parameters.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo     initial version
//
//=======================================================================================
#ifndef IMAGE_PARAMETERS_H
#define IMAGE_PARAMETERS_H

#include "stdio.h"


#define BYTES_PER_PIXEL 1
#define COLS 512 * BYTES_PER_PIXEL
#define ROWS 512 * BYTES_PER_PIXEL
#define SIZE ROWS* COLS
#define IMAGE_IN "lena512.bmp"           // Default image used
#define IMAGE_OUT "lena512_out.bmp"
#define IMAGE_IN_CHECK "barbara.bmp"
#define IMAGE_OUT_CHECK "barbara_out.bmp"

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

#endif

