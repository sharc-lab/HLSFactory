//========================================================================================
// 
//
// File Name    : tb_snow_3G.h
// Description  : Testbench
// Release Date : 26/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author             Description
//---------------------------------------------------------------------------------------
//26/07/2013   1.0      PolyU DARC Lab     snow 3G encryption testbench module declaration
//=======================================================================================


#ifndef TB_SNOW_3G_H
#define TB_SNOW_3G_H

#include "define.h"

SC_MODULE (test_snow_3G){

  // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<32> >  ks_out[SIZE];

  // outputs
  sc_out<sc_uint<32> >  ks_in[SIZE];




  //For data feeding
  FILE * in_file,  *out_golden_file, *out_file;
  FILE  *out_snow_file, *diff_file;


  void send();
  void recv();
  void compare_results();


  //clk("clk"),
     //rst("rst"),
     // in_data("in_data"),
     // coeff("coeff"),
     //    filter_output("filter_output")

  SC_CTOR ( test_snow_3G ) {
 
    SC_CTHREAD(send,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);
  };

  ~test_snow_3G(){};

};


#endif // TB_SNOW_3G_H
