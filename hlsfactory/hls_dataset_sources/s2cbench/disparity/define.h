//========================================================================================
// 
// File Name    : define.h
// Description  : Main definition header file for disparity
// Release Date : 23/02/2018
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 23/02/2018    1.0           DARClab          definition file for disparity
//=======================================================================================

#ifndef DEFINE_H_
#define DEFINE_H_
#include "systemc.h"
/***************************************************************
* Macro Definitions
***************************************************************/
#define IMAGE_NAME  "scene"
#define IMAGE_EXT   ".bmp"
#define IMAGE_L     (IMAGE_NAME "_l" IMAGE_EXT)
#define IMAGE_R     (IMAGE_NAME "_r" IMAGE_EXT)
#define IMAGE_DIS   (IMAGE_NAME "_out" IMAGE_EXT)
#define IMAGE_GOLDEN (IMAGE_NAME "_out_golden" IMAGE_EXT)
#define DIFFFILENAME "diff.txt"

#define WIN_SIZE    5 //must be odd number
#define MAX_SHIFT   32
#define IN_BUFFER_DEPTH    512

#define HIST_BIN 256  // color depth for histogram_equalization

/***************************************************************
* Structure Definitions
***************************************************************/
typedef unsigned char       U8;
typedef unsigned short int  U16;
typedef unsigned int        U32;

// BMP file Header structure
#pragma pack(push, 1)
typedef struct
{
    U16 type;              // file type
    U32 size;              // file size in bytes
    U16 reserved1;
    U16 reserved2;
    U32 offset;            // offset to image data, bytes
}BmpFileHeader;
#pragma pack(pop)

// BMP file information structure after Header
#pragma pack(push, 1)
typedef struct
{
    U32 size;                   // Header size in bytes
    U32 width;                  // image width
    U32 height;                 // image height
    U16 planes;                 // number of color planes
    U16 bits;                   // bits per pixel
    U32 compression;            // compression type
    U32 image_size;             // image size in bytes
    U32 x_resolution;           // number of pixels per meter in x
    U32 y_resolution;           // number of pixels per meter in y
    U32 n_colors;               // number of colors used
    U32 important_colors;       // number of important colors
}BmpInfoHeader;
#pragma pack(pop)



#endif
