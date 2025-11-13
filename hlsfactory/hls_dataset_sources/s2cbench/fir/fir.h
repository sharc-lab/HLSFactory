//========================================================================================
// 
// File Name    : fir.h
// Description  : FIR filter module declaration
// Release Date : 14/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//14/02/2013      1.0       PolyU     FIR filter module declaration
//=======================================================================================

#ifndef FIR_H_
#define FIR_H_
#include "define.h"


SC_MODULE (fir) {

	public:

	// Inputs
	sc_in_clk clk;
	sc_in<bool> rst;
	sc_in<sc_uint<8> > in_data;
	sc_in<sc_int<8> > coeff[FILTER_TAPS] ;

	// Output
	sc_out< sc_uint<24> > filter_output ;


	/* F */
	void fir_main ( void );
	sc_uint<24> filter(sc_uint<8> *, sc_int<8> *);


	// Constructor
	SC_CTOR (fir) {

	   SC_CTHREAD (fir_main, clk.pos() );
	   reset_signal_is(rst, false) ;
	   sensitive << clk.pos();

	}

	// Destructor
	~fir() {}


};


#endif   //  FIR_H_

