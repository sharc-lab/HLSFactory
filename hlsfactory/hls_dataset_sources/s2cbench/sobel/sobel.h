//========================================================================================
// 
// File Name    : sobel.h
// Description  : SOBEL filter declaration
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author              Description
//---------------------------------------------------------------------------------------
//23/07/2013       1.0     PolyU DARC Lab      main component declaration of SOBEL filter
//=======================================================================================

#ifndef SOBEL_H_
#define SOBEL_H_


#include "define.h"


SC_MODULE (sobel){
    
 public:
  
  sc_in_clk clk; //clock
  sc_in<bool> rst; //reset

  //inputs
  sc_in <sc_uint <8> > input_row[SIZE_BUFFER];
 
  //outputs
 sc_out<sc_uint<8> > output_row;


 // Variables declarations
  sc_uint<8>  line_buffer[SIZE_BUFFER][SIZE_BUFFER];

  //function prototypes
 
 /* S */
 sc_uint<8> sobel_filter(sc_uint<8> *);
 void sobel_main(void);
 
 SC_CTOR (sobel){
    SC_CTHREAD(sobel_main,clk.pos());
    reset_signal_is(rst,false); 
 };

 ~sobel(){};



};
   
#endif  // SOEBEL_H_
