//========================================================================================
// 
// File Name    : huffman.h
// Description  : Huffman header
// Release Date : 30/5/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author         Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0   UTD DARClab     Huffman header file  
//=======================================================================================
#ifndef HUFFMAN_H_
#define HUFFMAN_H_
#include "define.h"

SC_MODULE (huffman){

  public:

    sc_in_clk clk; //clock
    sc_in<bool> rst; //reset

    //inputs
    sc_in <sc_int<QUAN_OUT_WIDTH> > huffman_in[RLE_BUFF_LEN];
    sc_in <sc_uint<8> > rle_length;

    //outputs
    sc_out <sc_uint<1> > huffman_out[64];
    sc_out <sc_uint<9> > last;

    // variable declarations
    sc_uint<1> encode_output[512];
    sc_uint<9> last_bit;
    sc_uint<1> check_byte[8];
    sc_uint<3> check_index;

    // functions
    void huffman_main(void);
    sc_uint<4> jpeg_getCat(sc_int<QUAN_OUT_WIDTH> a);
    void jpeg_DCcode(sc_int<QUAN_OUT_WIDTH> rl0);
    void jpeg_ACcode(sc_int<QUAN_OUT_WIDTH> rl[64], sc_uint<8> length);
    void jpeg_addzero(void);

    SC_CTOR (huffman){
      SC_CTHREAD(huffman_main,clk.pos());
      reset_signal_is(rst,false);
    };

    ~huffman(){}

};
#endif

