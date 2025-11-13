//========================================================================================
// 
//
// File Name    : tb_decim.h
// Description  : Testbench
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author       Description
//---------------------------------------------------------------------------------------
// 31/07/2013   1.0      PolyU       Decimation filter stimuli generation module
//=======================================================================================

#ifndef TEST_DECIM
#define TEST_DECIM

#define SC_INCLUDE_FX 
#include "define.h"

SC_MODULE (test_decim){

  // Inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in < sc_fixed<19, 2, SC_RND, SC_SAT> > odata;

  // Outputs
   sc_out<bool> load_coeff;  
   sc_out < sc_fixed<19,2,SC_RND,SC_SAT> > indata;

   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef1[TAPS_STAGE1] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef2[TAPS_STAGE2] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef3[TAPS_STAGE3] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef4[TAPS_STAGE4] ;
   sc_out< sc_fixed<17,2,SC_RND,SC_SAT> > incoef5[TAPS_STAGE5] ;


  //For data feeding

  FILE * in_filter_file, *in_coeff_file, *out_filter_golden_file, *out_filter_file_read;
  FILE  *out_filter_file, *diff_file;


  /* C */
  void compare_results();

  /* R */
  void recv();

  /* S */
  void send();




  SC_CTOR ( test_decim ) {
 
    SC_CTHREAD(send,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);

    SC_CTHREAD(recv,clk.pos());
    sensitive<<rst.neg();
    reset_signal_is(rst,false);
  };

  ~test_decim(){};

};

#endif // TEST_DECIM
