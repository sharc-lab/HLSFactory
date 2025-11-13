//========================================================================================
// 
// File Name    : tb_ave8.h
// Description  : Testbench
// Release Date : 12/08/2017
// Author       : DARClab, UTD
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//12/08/2017      1.0      DARClab     ave8 testbench
//=======================================================================================


#include "define.h"


#ifndef TB_AVE8_H_
#define TB_AVE8_H_

SC_MODULE (test_AVE8){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<8> > ave8_output; 

  // Outputs
  sc_out<sc_uint<8> >  in_data;



  //File pointers
  FILE * in_ave8_file, *out_ave8_golden_file, *out_ave8_file_read;
  FILE  *out_ave8_file, *diff_file;

  
  /* C */
  void compare_results();

  /* R  */
  void recv();

  /* S */
  void send();


  SC_CTOR ( test_AVE8 ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  }

  ~test_AVE8(){}

};


#endif  // TB_AVE8_H
