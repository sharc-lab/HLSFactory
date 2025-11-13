//========================================================================================
// 
// File Name    : tb_fir.h
// Description  : Testbench
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//14/02/2013      1.0       PolyU     FIR filter original testbench
//=======================================================================================
#ifndef TB_FIR_H_
#define TB_FIR_H_
#include "define.h"


SC_MODULE (test_FIR){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<24> > filter_output; 

  // Outputs
  sc_out<sc_uint<8> >  in_data;
  sc_out<sc_int<8> > coeff[FILTER_TAPS];


  //File pointers
  FILE * in_filter_file, *in_coeff_file, *out_filter_golden_file, *out_filter_file_read;
  FILE  *out_filter_file, *diff_file;

  
  /* C */
  void compare_results();

  /* R  */
  void recv();

  /* S */
  void send();


  SC_CTOR ( test_FIR ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  }

  ~test_FIR(){}

};


#endif  // TB_FIR_H
