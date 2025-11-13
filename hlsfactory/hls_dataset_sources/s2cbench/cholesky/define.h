//========================================================================================
// 
//
// File Name    : define.h
// Description  : definition file
// Release Date : 12/04/2017
// Author       : UTD DARClab
//                Benjamin Carrion Schafer 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version    Author       Description
//----------------------------------------------------------------------------------------
//12/04/2017       1.0     UTD (BCS)    definition file
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#define SC_INCLUDE_FX

#include "systemc.h"

//#include <iostream>
#include "stdio.h"



#define MATRIX_SIZE 9		// 9=3x3 matrix,  16=4x4, etc..
#define MATRIX_DIM 3		// 3 in3x3 matrix,  4 in =4x4, etc..

#define INFILENAME         "cholesky_in_data.txt"

#define OUTFILENAME_GOLDEN       "cholesky_output_golden.txt"
#define OUTFILENAME              "cholesky_output.txt"
#define DIFFFILENAME             "choleskydiff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif

