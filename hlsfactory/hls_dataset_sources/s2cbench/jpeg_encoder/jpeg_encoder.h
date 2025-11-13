//========================================================================================
// 
// File Name    : jpeg_encoder.h
// Description  : JPEG encoder main header file
// Release Date : 30/5/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author         Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0   UTD DARClab     JPEG encoder top module
//=======================================================================================
#ifndef JPEG_ENCODER_H_
#define JPEG_ENCODER_H_

#include "define.h"
#include "dct.h"
#include "quantization.h"
#include "rle.h"
#include "huffman.h"

SC_MODULE (jpeg_encoder){

  public:
    sc_in_clk clk; //clock
    sc_in<bool> rst; //reset

    //inputs
    sc_in <sc_uint<8> > input_col[8];

    //outputs
    sc_out <sc_uint<1> > output[64];
    sc_out <sc_uint<9> > last;

    sc_signal<sc_int<DCT_OUT_WIDTH> > dct_out[64];
    sc_signal<sc_int<QUAN_OUT_WIDTH> > quantization_out[64];
    sc_signal<sc_int<QUAN_OUT_WIDTH> > rle_out[RLE_BUFF_LEN];
    sc_signal<sc_uint<8> > rle_length;
    
    //instance declaration
    dct *u_DCT;
    quantization *u_QUANTIZATION;
    rle *u_RLE;
    huffman *u_HUFFMAN;

    SC_CTOR (jpeg_encoder){

      int i;

      u_DCT = new dct("DCT");
      u_QUANTIZATION = new quantization("QUANTIZATION");
      u_RLE = new rle("RLE");
      u_HUFFMAN = new huffman("HUFFMAN");

      //connect to instancies
      u_DCT->clk( clk );
      u_DCT->rst( rst );
      for(i=0;i<8;i++)
        u_DCT->dct_in[i]( input_col[i] );
      for(i=0;i<64;i++)
        u_DCT->dct_out[i]( dct_out[i] );

      u_QUANTIZATION->clk( clk );
      u_QUANTIZATION->rst( rst );
      for(i=0;i<64;i++){
        u_QUANTIZATION->quantization_in[i]( dct_out[i] );
        u_QUANTIZATION->quantization_out[i]( quantization_out[i] );
      }

      u_RLE->clk( clk );
      u_RLE->rst( rst );
      for(i=0;i<64;i++)
        u_RLE->rle_in[i]( quantization_out[i] );
      for(i=0;i<RLE_BUFF_LEN;i++)
        u_RLE->rle_out[i]( rle_out[i] );

      u_RLE->rle_length( rle_length );

      u_HUFFMAN->clk( clk );
      u_HUFFMAN->rst( rst );
      for(i=0;i<64;i++)
        u_HUFFMAN->huffman_out[i]( output[i] );
      for(i=0;i<RLE_BUFF_LEN;i++)
        u_HUFFMAN->huffman_in[i]( rle_out[i] );      
   
      u_HUFFMAN->rle_length( rle_length ); 
      u_HUFFMAN->last( last );
    }
};

#endif


