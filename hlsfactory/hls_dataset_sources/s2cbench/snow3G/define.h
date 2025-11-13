//========================================================================================
// 
//
// File Name    : define.h
// Description  : Main definition header file for snow 3G
// Release Date : 14/02/2013
// Author       : ETSI/SAGE (www.gsm.com)
// Modified     : PolyU DARC Lab
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author                      Version     Description
//----------------------------------------------------------------------------------------
// 2006        1.0    ETSI/SAGE        Original ANSI C description
//26/07/13     1.1    PolyU DARC Lab   Converted into Synthesizable SystemC
//=======================================================================================

#ifndef DEFINE_H
#define DEFINE_H

#include "systemc.h"

//#include <iostream>
#include "stdio.h"


#define SIZE 4

#define INFILENAME               "snow_3G_input.txt"  // random

#define OUTFILENAME_GOLDEN       "snow_3G_output_golden.txt"
#define OUTFILENAME              "snow_3G_output.txt"
#define DIFFFILENAME             "snow_3G_diff.txt"

//#define WAVE_DUMP          // set do dump waveform or set as compile option -DWAVE_DUMP

#endif  // DEFINE_H

