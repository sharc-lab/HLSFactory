//========================================================================================
// 
// File Name    : tb_top.h
// Description  : Testbench
// Release Date : 10/02/2015
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Xu Siyuan
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//10/02/2015      1.1       PolyU     top vga original testbench
//=======================================================================================


#include "define.h"


#ifndef TB_TOP_H_
#define TB_TOP_H_

SC_MODULE (test_top){

  // Inputs
  sc_in<bool>          sys_clk;
  sc_in<bool>          reset_n;

   sc_in< bool >  h_sync ;//--horiztonal sync pulse
   sc_in< bool >  v_sync ;//--vertical sync pulse
   sc_in< bool >   n_blank  ;//--direct blacking output to DAC
   sc_in< bool >   n_sync  ;//--sync-on-green output to DAC
   sc_in <sc_uint<8> > red; //red magnitude output to DAC
   sc_in <sc_uint<8> > green; //green magnitude output to DAC
   sc_in<sc_uint<8> > blue; //blue magnitude output to DAC


  //File pointers

  FILE *out_h_sync_file, * out_v_sync_file,*out_n_blank_file,*out_n_sync_file, *out_red_file, *out_blue_file, *out_green_file; 
  

  /* R  */
  void recv();



  SC_CTOR ( test_top ) {
 

    SC_CTHREAD(recv,sys_clk.pos());
    reset_signal_is(reset_n,false);
  }

  ~test_top(){}

};


#endif  // TB_TOP_H
