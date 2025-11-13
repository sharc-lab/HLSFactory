//========================================================================================
// 
// File Name    : jpeg_encoder.cpp
// Description  : JPEG encoder top module
// Release Date : 30/05/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     JPEG encoder
//=======================================================================================


#include "define.h"
#include "jpeg_encoder.h"
#include "dct.cpp"
#include "quantization.cpp"
#include "rle.cpp"
#include "huffman.cpp"
