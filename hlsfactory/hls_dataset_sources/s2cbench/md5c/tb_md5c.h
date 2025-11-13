//========================================================================================
// 
//
// File Name    : tb_md5c.h
// Description  : Testbench
// Release Date : 29/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version  Author          Description
//---------------------------------------------------------------------------------------
//29/07/2013     1.0     PolyU DARC Lab  MD5C original testbench
//=======================================================================================


#include "define.h"

SC_MODULE (test_md5c){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in< bool >         o_busy;
  sc_in< bool >         o_start;
  sc_in< sc_uint<8> >   o_digest;


  //  Outputs
  sc_out< bool >          i_req;
  sc_out< bool >          i_final;
  sc_out< sc_uint<32> >   i_inputLen;




  //For data feeding
  FILE * in_file, *out_file_golden, *out_file, *diff_file;


  /* C */
  void compare_results();

  /* R */
  void recv();

  /* S */
  void send();



  SC_CTOR ( test_md5c ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  };

  ~test_md5c(){};

};
