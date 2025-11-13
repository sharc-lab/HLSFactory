//========================================================================================
// 
//
// File Name    : tb_aes_cipher.h
// Description  : Testbench
// Release Date : 29/02/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author      Description
//---------------------------------------------------------------------------------------
//14/02/2013    1.0     PolyU         AES Cipher  testbench module description
//=======================================================================================


#ifndef TB_AES_H
#define TB_AES_H

#include "define.h"

SC_MODULE (test_aes){

	// inputs
	sc_in<bool>          clk;
	sc_in<bool>          rst;
	sc_in<sc_uint<8> >   odata[SIZE];

	// outputs
	sc_out<sc_uint<8> >   idata[SIZE];
	sc_out<sc_uint<8> >   ikey[SIZE];

	//For data feeding
	FILE * in_file, *in_file_key,  *out_golden_file, *out_file;
	FILE  *out_aes_cipher_file, *diff_file;

	sc_uint<8>  input_key[SIZE];

	/* C */
	void compare_results();

	/* R */
	void recv();

	/* S */
	void send();

	SC_CTOR ( test_aes ) {

		SC_CTHREAD(send,clk.pos());
		reset_signal_is(rst,false);

		SC_CTHREAD(recv,clk.pos());
		reset_signal_is(rst,false);
	}

	~test_aes(){}

};


#endif // TB_AES_H
