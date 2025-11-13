//========================================================================================
// 
// File Name    : tb_adpcm_encoder.h
// Description  : Testbench for adpcm encoder
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra  
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version   Author           Description
//---------------------------------------------------------------------------------------
//31/07/2013    1.0    PolyU DARC Lab   ADPCM encoder testbench module declaration
//=======================================================================================

#ifndef TEST_ADPCM
#define TEST_ADPCM

#include "define.h"

SC_MODULE (test_adpcm){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<4> > odata;

  //  Outputs
  sc_out<sc_uint<16> > idata;


  //For data feeding
  FILE * in_file, *out_file_golden, *out_file, *diff_file;


  /* C */
  void compare_results();

  /* R */
  void recv();

  /* S */
  void send();


  SC_CTOR ( test_adpcm ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  };

  ~test_adpcm(){};

};

#endif // TEST_ADPCM
