//========================================================================================
// 
// File Name    : quantization.h
// Description  : Quantization module header file
// Release Date : 30/5/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author         Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0   UTD DARClab     Quantization module header file
//=======================================================================================

#ifndef QUANTIZATION_H_
#define QUANTIZATION_H_
#include "define.h"

SC_MODULE (quantization){

  public:

    sc_in_clk clk; //clock
    sc_in<bool> rst; //reset

    //input
    sc_in <sc_int<DCT_OUT_WIDTH> > quantization_in[64];

    //output
    sc_out <sc_int<QUAN_OUT_WIDTH> > quantization_out[64];

    //function
    void jpeg_quantization(void);

    SC_CTOR (quantization){
      SC_CTHREAD(jpeg_quantization,clk.pos());
      reset_signal_is(rst,false);
    };

    ~quantization(){}

};

#endif
