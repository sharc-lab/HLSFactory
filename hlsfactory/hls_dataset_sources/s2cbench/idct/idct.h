//========================================================================================
// 
//
// File Name    : idct.h
// Description  : IDCT
// Release Date : 31/07/2013
//
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author      Version      Description
//----------------------------------------------------------------------------------------
//  1991-1998  Thomas G. Lange 1.0      Origiinal IDCT implementation    
//  31/07/2013  PolyU          1.1      Converted into Synthesizable SystemC                 
//
//=======================================================================================


#ifndef IDCT_H_
#define IDCT_H_

#include "define.h"

SC_MODULE( idct ) {

  typedef int        workspace_t;

  // Inputs
  sc_in_clk              clk;
  sc_in< bool >          rst;

  sc_in<  bool >         input_ready;
  sc_in< sc_int<16>  >   input_coef;
  sc_in< sc_uint<6>  >   input_quant;

  // Outputs
  sc_out< bool >         output_start;
  sc_out< sc_uint<8>  >  output_sample;
  sc_out< sc_uint<3> >   output_row;
  sc_out< sc_uint<3> >   output_col;
  
  /* E */
  void entry();

  /* J */
  void jpeg_idct_islow();

  /* R */
  void run();
  


  SC_CTOR( idct ) {
      SC_CTHREAD(run, clk.pos());
      reset_signal_is(rst, false);
  };



    ~idct(){};

};

#endif // IDCT_H_
