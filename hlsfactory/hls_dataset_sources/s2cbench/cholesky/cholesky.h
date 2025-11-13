//========================================================================================
// 
// File Name    : cholesky.h
// Description  : cholesky decomposition  module declaration
// Release Date : 12/04/2017
// Author       : UTD DARClab
//                Benjamin Carrion Schafer 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//12/04/2017      1.0      UTD     cholesky decompositon module declaration
//=======================================================================================

#ifndef CHOLESKY_H_
#define CHOLESKY_H_


#include "define.h"


SC_MODULE (cholesky) {


public:

   // Inputs
   sc_in_clk clk;
   sc_in<bool> rst;
    sc_in<sc_fixed<16,8,SC_RND,SC_SAT> >in_data[MATRIX_SIZE] ;

   // Output
  sc_out<sc_fixed<16,8,SC_RND,SC_SAT> > cholesky_output[MATRIX_SIZE];


  /* Global variables */
 sc_fixed<16,8,SC_TRN,SC_SAT> L[MATRIX_SIZE];	


   /* C */
   void cholesky_main ( void );
   void cholesky_compute(sc_fixed<16,8,SC_RND,SC_SAT>* );


   // Constructor
 SC_CTOR (cholesky) {

       SC_CTHREAD (cholesky_main, clk.pos() );
       reset_signal_is(rst, false) ;
       sensitive << clk.pos();

   }

   // Destructor
   ~cholesky() {}


};


#endif   //  CHOLESKY_H_

