//========================================================================================
// 
// File Name    : tb_jpeg_encoder.h
// Description  : JPEG encoder testbench header file
// Release Date : 30/5/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version   Author         Description
//---------------------------------------------------------------------------------------
//23/07/2013      1.0   UTD DARClab     TB for JPEG encoder            
//=======================================================================================

#ifndef TB_JPEG_ENCODER_H_
#define TB_JPEG_ENCODER_H_

#include "define.h"


SC_MODULE (tb_jpeg_encoder){

 public:

  //inputs
  sc_in<bool>  clk;
  sc_in<bool> rst;
  sc_in<sc_uint<1> > output[64];
  sc_in<sc_uint<9> > last_bit;

  //outputs
  sc_out<sc_uint <8> > input_col[8];


  //variables
  int jpeg_len;
  int jpeg_len_b;
  unsigned char *bitmapData;
  unsigned char  *jpegFinalImage;
  bool *jpegFinalImage_bits;
  FILE *ifptr, *ofptr, *diffptr;
  BITMAPINFOHEADER bitmapInfoHeader;
  BITMAPFILEHEADER bitmapFileHeader; //our bitmap file header
  unsigned char biColourPalette[1024];
 

  //function prototypes
  /* C */
 void  compare_results();

  /* I */
  void image_write(void);

  /* L */
  unsigned char *load_bitmapfile(const char *);


  /* R */
  void recv();


  /* S */
  void send();

  unsigned char bitToByte(bool* bitarray);


  SC_CTOR( tb_jpeg_encoder){
  
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);
  
    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
    
  }

  ~tb_jpeg_encoder(){};

};

#endif  //  TB_SOBEL_H_

  
