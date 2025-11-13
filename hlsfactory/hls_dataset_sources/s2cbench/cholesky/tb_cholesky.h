//========================================================================================
// 
// File Name    : tb_cholesky.h
// Description  : Testbench header
// Release Date : 12/07/2017
// Author       : UTD DARClab
//                Benjamin Carrion Schafer 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//12/07/2017      1.0      UTD (BCS)   Cholesky decomposition  testbench
//=======================================================================================


#include "define.h"


#ifndef TB_CHOLESKY_H_
#define TB_CHOLESKY_H_

SC_MODULE (test_CHOLESKY){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_fixed<16,8,SC_RND,SC_SAT> > cholesky_output[MATRIX_SIZE]; 

  // Outputs
  sc_out<sc_fixed<16,8,SC_RND,SC_SAT> >  in_data[MATRIX_SIZE];



  //File pointers
  FILE * in_file, *out_golden_file, *out_file_read;
  FILE  *out_file, *diff_file;

  
  /* C */
  void compare_results();

  /* R  */
  void recv();

  /* S */
  void send();


  SC_CTOR ( test_CHOLESKY ) {
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
  }

  ~test_CHOLESKY(){}

};


#endif  // TB_CHOLESKY_H
