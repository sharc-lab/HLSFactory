//=============================================================================
// 
//
// File Name    : fft.h
// Description  : FFT implementation
// Release Date : 16/07/13
// Author       : 
//
// Revision History
//---------------------------------------------------------------------------
// Date      Version   Author        Description
//---------------------------------------------------------------------------
//               1.0 R.Goswami,Synopsys, Inc.  fft.cpp - This is the implementation file for the synchronous process "fft"
// 16/07/03      1.1  PolyU DARClab                    
//=============================================================================

/*****************************************************************************

  The following code is derived, directly or indirectly, from the SystemC
  source code Copyright (c) 1996-2006 by all Contributors.
  All Rights reserved.

  The contents of this file are subject to the restrictions and limitations
  set forth in the SystemC Open Source License Version 2.4 (the "License");
  You may not use this file except in compliance with such restrictions and
  limitations. You may obtain instructions on how to receive a copy of the
  License at http://www.systemc.org/. Software distributed by Contributors
  under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
  ANY KIND, either express or implied. See the License for the specific
  language governing rights and limitations under the License.

 *****************************************************************************/

#ifndef FFT
#define FFT


#include "define.h"



struct fft: sc_module {

// Inputs
  sc_in_clk clk;
  sc_in<bool> rst;   

  sc_in<sc_fixed<32,16, SC_RND, SC_SAT> > in_real;   
  sc_in<sc_fixed<32,16, SC_RND, SC_SAT> > in_imag;   
  sc_in<bool> data_valid; 
  sc_in<bool> data_ack;   

// Outputs
  sc_out<sc_fixed<32,16, SC_RND, SC_SAT> > out_real;        
  sc_out<sc_fixed<32,16, SC_RND, SC_SAT> > out_imag;        
  sc_out<bool> data_req;         
  sc_out<bool> data_ready;     
  

 
  SC_CTOR(fft){

      SC_CTHREAD(entry, clk.pos() );
      reset_signal_is(rst, false);

    };

// Destructor
  ~fft(){};  


void entry(void);


};      

#endif //FFT
