//========================================================================================
// 
//
// File Name    : define.h
// Description  : Definition file for IDCT
// Release Date : 31/07/2013
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author     Version     Description
//----------------------------------------------------------------------------------------
// 31/07/2013  PolyU       1.0       definition file
//=======================================================================================

#ifndef DEFINE_IDCT_H
#define DEFINE_IDCT_H

#include "systemc.h"

#include <iostream>
#include "stdio.h"

#define DCTSIZE 8
#define DCTSIZE2 64


#define INFILENAME             "idct_input.txt"   // random
#define INFILENAME_COEF        "idct_coef.txt"    // random
#define OUTFILENAME_GOLDEN     "idct_output_golden.txt"
#define OUTFILENAME            "idct_output.txt"


#define DIFFFILENAME           "idct_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // end DEFINE_IDCT_H

