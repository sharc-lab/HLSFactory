//========================================================================================
// 
// File Name    : disparity.h
// Description  : disparity module declaration
// Release Date : 23/02/2018
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 23/02/2018    1.0           DARClab          disparity module declaration
//=======================================================================================

#ifndef DISPARITY_H_
#define DISPARITY_H_

#include "define.h"

SC_MODULE (disparity){

public:
	// Inputs
	sc_in_clk clk;
	sc_in<bool> rst;
	
	sc_in <sc_uint<8> > data_in_r; // pixel values of right image
	sc_in <sc_uint<8> > data_in_l; // pixel values of left image
	sc_in <sc_uint<16> > width; //width of the image(include margin) 
	
	// Output
	sc_out <sc_uint<8> > data_out;
	sc_out <bool> ready; // ready = 1 when data_out is ready, just to make testbench easier.
	
	// variables
	sc_uint<16> width_read;
	sc_uint<8> in_buffer_r[IN_BUFFER_DEPTH][WIN_SIZE];
	sc_uint<8> in_buffer_l[IN_BUFFER_DEPTH][WIN_SIZE];
	sc_uint<8> image_diff[IN_BUFFER_DEPTH][WIN_SIZE];
	sc_uint<8> image_win_sad[IN_BUFFER_DEPTH];
	sc_uint<8> image_min_sad[IN_BUFFER_DEPTH];
	sc_uint<8> output_buffer[IN_BUFFER_DEPTH];
	
	// functions
	void shift_diff(sc_uint<8> image_in_r[IN_BUFFER_DEPTH][WIN_SIZE],
		sc_uint<8> image_in_l[IN_BUFFER_DEPTH][WIN_SIZE], sc_uint<16> width, int shift);
	
	void window_sad(sc_uint<16> width);

	void find_disparity(sc_uint<16> width, int shift);
	
	void disparity_main(void);
	
	// constructor
	SC_CTOR (disparity) {
		SC_CTHREAD (disparity_main, clk.pos());
		reset_signal_is(rst, false);
	}
	
	~disparity(){}
};

#endif
