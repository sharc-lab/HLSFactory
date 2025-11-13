//========================================================================================
// 
// File Name    : tb_interp.h
// Description  : Header for the testbench
// Release Date : 23/02/2018
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra, Jianqi Chen 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date           Version   Author     Description
//----------------------------------------------------------------------------------------
// 23/07/2013        1.0    PolyU      TB header file
// 23/02/2018        1.1    UTD        increase number of data outputs to 4
//=======================================================================================
#ifndef TB_INTERP_
#define TB_INTERP_


#define SC_INCLUDE_FX

#include "define.h"


SC_MODULE(tb_interp){

	public:


	//inputs  
	sc_in_clk clk;
	sc_in<bool> rst;


	sc_in<bool> odata_en;
	sc_in<sc_fixed <16,2,SC_RND,SC_SAT> > odata[4];

	//outputs

	sc_out<sc_fixed <16,2,SC_TRN,SC_WRAP> > indata;

	// Variables declaration
	FILE *fptr_data;
	FILE *fptr_en;
	FILE *in_file_data, *in_file_factor;


	//function prototypes

	/* C */
	void compare_results(void);

	/* R */
	void recv(void);

	/* S */
	void send(void);



	SC_CTOR(tb_interp){

		SC_CTHREAD(send,clk.pos());
		reset_signal_is(rst, false);

		SC_CTHREAD(recv,clk.pos());
		reset_signal_is(rst, false);

	}

	~tb_interp(){}


};

#endif  // TB_INTERP
