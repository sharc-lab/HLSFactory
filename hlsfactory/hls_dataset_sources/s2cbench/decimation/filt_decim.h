//========================================================================================
// 
// File Name    : filter_decim.h
// Description  : Main 5 stages decimation filter component declaration
// Release Date : 31/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version   Author      Description
//----------------------------------------------------------------------------------------
//31/07/2013      1.0    PolyU      Decimation filter module declaration
//=======================================================================================

#ifndef DECFILT_H_
#define DECFILT_H_

#define SC_INCLUDE_FX 
#include "define.h"


SC_MODULE(decfilt)
 {


public:
   
   // Inputs
   
   sc_in_clk clk;   
   sc_in<bool> rst;

   sc_in<bool> load_coeff;    
   sc_in < sc_fixed<19,2,SC_RND,SC_SAT> > indata;

   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef1[TAPS_STAGE1] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef2[TAPS_STAGE2] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef3[TAPS_STAGE3] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef4[TAPS_STAGE4] ;
   sc_in< sc_fixed<17,2,SC_RND,SC_SAT> > incoef5[TAPS_STAGE5] ;

   // Output   
   sc_out < sc_fixed<19, 2, SC_RND, SC_SAT> > odata;

   // Variables
   int stage_counter_1;   // decimation stages counter. HLS toold should 
   int stage_counter_2;   // adust bitwidth automatically
   int stage_counter_3;
   int stage_counter_4;
   int stage_counter_5;
   
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_1[7];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_2[7];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_3[7];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_4[11];
   sc_fixed<19,2,SC_RND,SC_SAT> bufferline_5[24];


   // Functions declarations
   /*  R  */
   void run ( void );


 SC_CTOR (decfilt) {
   SC_CTHREAD (run, clk.pos()); 
   reset_signal_is(rst, false);
 };
 

   ~decfilt() {};


};

#endif   // END DECFILT_
