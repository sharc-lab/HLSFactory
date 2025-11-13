//========================================================================================
// 
//
// File Name    : define.h
// Description  : Definition file for Md5C
// Release Date : 30/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version Author                Description
//----------------------------------------------------------------------------------------
//30/07/2013     1.0   PolyU DARC Lab        definition file
//=======================================================================================

#ifndef DEFINE_FFT_H
#define DEFINE_FFT_H

#include "systemc.h"

#include <iostream>
#include "stdio.h"




#define MD5C_INPUT_BUFSIZE   1024
#define MD5C_INPUT_BUFSIZE_T sc_uint<10>


#define INFILENAME             "md5c_input.txt"      // random
#define OUTFILENAME_GOLDEN     "md5c_output_golden.txt"
#define OUTFILENAME            "md5c_output.txt"


#define DIFFFILENAME                 "md5c_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // end DEFINE_FFT_H

