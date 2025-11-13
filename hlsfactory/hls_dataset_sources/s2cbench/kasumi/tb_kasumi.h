//========================================================================================
// 
//
// File Name    : tb_kasumi.h
// Description  : Testbench
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author        Description
//---------------------------------------------------------------------------------------
//23/07/2013     1.0    PolyU        Kasumi  testbench
//=======================================================================================


#ifndef TB_KASUMI_H
#define TB_KASUMI_H

#include "define.h"

SC_MODULE (test_kasumi){

  // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<8> >  odata[DEPTH][WIDTH];

  // outputs
  sc_out<sc_uint<8> >  indata[DEPTH][WIDTH];
  sc_out<sc_uint<8> >  k[8][2];


  //For data feeding
  FILE * in_file, *in_kfile,  *out_golden_file, *out_kasumi_file;
  FILE  *out_file, *diff_file;


  void send();
  void recv();
  void compare_results();


  SC_CTOR ( test_kasumi ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  };

  ~test_kasumi(){};

};


#endif // TB_KASUMI_H
