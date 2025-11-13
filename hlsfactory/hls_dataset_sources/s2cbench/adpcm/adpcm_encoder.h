//========================================================================================
// 
//
// File Name    : adpcm_encoder.h
// Description  : ADPCM encoder module declaration
// Release Date : 31/07/2013
//
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version  Author       Description
//---------------------------------------------------------------------------------------
// 2001-2003      1.0    ffmeg project   ADPCM  original ANSI Cdescription
// 31/07/2013     1.1     PolyU          ADPCM moddule declaration
//=======================================================================================

#include "define.h"


#ifndef ADPCM_H
#define ADPCM_H

SC_MODULE(adpcm) {

    // Inputs
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<sc_uint<16> > idata;

    // Outputs
    sc_out<sc_uint<4> > odata;


    // Methods

    /* D */
    sc_uint<32> div_mod( sc_uint<18>, sc_uint<15> );

    /* G  */
    sc_uint<4> get_index_delta( sc_uint<4> );

    /* R */
    void run();


    SC_CTOR(adpcm) {
        SC_CTHREAD(run, clk.pos());
	reset_signal_is(rst, false);
    };

    ~adpcm(){};


};


#endif // ADPCM_H
