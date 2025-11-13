//========================================================================================
// 
// File Name    : kasumi.h
// Description  : Kasumi encryption core top module
// Release Date : 23/07/2013
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Author   Version     Description
//----------------------------------------------------------------------------------------
// 2006    ETSI/SAGE    1.0         Original ANSI C description
//23/07/13  PolyU       1.1         Converted into Synthesizable SystemC
//                                  (converted unions into structures)
//=======================================================================================

#ifndef KASUMI_H_
#define KASUMI_H_


#include "define.h"

SC_MODULE (kasumi) {
 
 // inputs
  sc_in<bool>          clk;
  sc_in<bool>          rst;

  sc_in<sc_uint<8> >  indata[DEPTH][WIDTH];
  sc_in<sc_uint<8> >  k[8][2];

  // outputs
  sc_out<sc_uint<8> >  odata[DEPTH][WIDTH];


/* Globals Variables: The subkey arrays */
 u16 KLi1[8], KLi2[8];
 u16 KOi1[8], KOi2[8], KOi3[8];
 u16 KIi1[8], KIi2[8], KIi3[8];

 // Function prototypes

 /* F */
 u16  FI( u16, u16);
 u32  FO( u32 , int);
 u32  FL( u32 , int );


 /* K */
 void kasumi_run();
 void keyschedule_run();


 SC_CTOR (kasumi){
  
   SC_CTHREAD(kasumi_run, clk.pos());
   reset_signal_is(rst, false);
   
   SC_CTHREAD(keyschedule_run, clk.pos());
   reset_signal_is(rst, false);

 };

  ~kasumi(){};


};


#endif // KASUMI_H

