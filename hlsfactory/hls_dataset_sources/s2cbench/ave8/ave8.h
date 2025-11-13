//========================================================================================
// 
// File Name    : ave8.h
// Description  : ave8 module declaration
// Release Date : 12/08/2017
// Author       : DARClab
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//12/08/2017      1.0      DARClab    ave8 module declaration
//=======================================================================================

#ifndef AVE8_H_
#define AVE8_H_


#include "define.h"


SC_MODULE (ave8) {


public:

   // Inputs
   sc_in_clk clk;
   sc_in<bool> rst;
   sc_in<sc_uint<8> > in_data ;

   // Output
   sc_out< sc_uint<8> > ave8_output ;


   /* F */
   void ave8_main ( void );


   // Constructor
 SC_CTOR (ave8) {

       SC_CTHREAD (ave8_main, clk.pos() );
       reset_signal_is(rst, false) ;
       sensitive << clk.pos();

   }

   // Destructor
   ~ave8() {}


};


#endif   //  AVE8_H_

