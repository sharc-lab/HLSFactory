//========================================================================================
// 
// File Name    : synth_param.h
// Description  : 
// Release Date : 22/10/2014
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 2014          1.0            David Aledo     Synthesis parameters Bitwidth
//
//=======================================================================================

#ifndef SYNTH_PARAM_H
#define SYNTH_PARAM_H

/* Synthesizable parameters */
#define NbitW 16   // bw of weights and bias
#define NbitIn 8   // bw of ANN input
#define NbitOut 8  // bw of ANN output

#define trunc_out 14 // Truncate this bits from the output of each layer,
// in order to place the decimal dot.

#endif

