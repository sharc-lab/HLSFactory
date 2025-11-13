//========================================================================================
// 
// File Name    : filter_interp.h
// Description  : Main interpolation filter component declaration
// Release Date : 23/02/2018
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra, Jianqi Chen
//
// Revision History
//---------------------------------------------------------------------------------------
// Date       Version   Author      Description
//----------------------------------------------------------------------------------------
//23/07/2013   1.0      PolyU       Interpolation filter declaration file
//23/02/2018   1.1      UTD         increase number of data outputs to 4, remove factor inputs
//=======================================================================================

#ifndef INTERP_H_
#define INTERP_H_

#define SC_INCLUDE_FX

#include "define.h"


SC_MODULE (interp) {

public:

	// Inputs
	sc_in_clk clk;
	sc_in<bool> rst;

	// Inputs
	sc_in< sc_fixed<16,2,SC_TRN,SC_WRAP> > indata;

	// Output   
	sc_out< bool > odata_en;
	sc_out< sc_fixed<16,2,SC_RND,SC_SAT> > odata[4];

	// Variables
	sc_fixed<16,2,SC_TRN,SC_WRAP> buffer[TAPS] ;

	sc_fixed<16,2,SC_TRN,SC_WRAP> indata_read;
	sc_ufixed<9,0,SC_TRN,SC_WRAP> infactor_read;
	sc_fixed<16,2,SC_RND,SC_SAT> odata_write;


	sc_fixed<36,6,SC_TRN,SC_WRAP> SoP1;
	sc_fixed<36,6,SC_TRN,SC_WRAP> SoP2;
	sc_fixed<36,6,SC_TRN,SC_WRAP> SoP3;
	sc_fixed<36,6,SC_TRN,SC_WRAP> SoP4;

	// Functions declarations


	/* R */
	void run (void);


	SC_CTOR (interp) {
	  SC_CTHREAD (run, clk.pos());
	  reset_signal_is (rst, false );
	}

	~interp() {}


}; 

#endif //  INTERP_H_
