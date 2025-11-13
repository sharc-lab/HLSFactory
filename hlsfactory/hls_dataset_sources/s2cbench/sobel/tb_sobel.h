//========================================================================================
// 
// File Name    : tb_sobel.h
// Description  : Testbench for SOBEL filter
// Release Date : 23/07/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version  Author           Description
//---------------------------------------------------------------------------------------
//23/07/2013       1.0  PolyU DARC Lab  ssobel filter testbench module declaration
//=======================================================================================

#ifndef TB_SOBEL_H_
#define TB_SOBEL_H_

#include "define.h"


SC_MODULE (tb_sobel){

 public:

  //inputs
  sc_in<bool>  clk;
  sc_in<bool> rst;
  sc_in<sc_uint<8> > output_row;
  

  //outputs
  sc_out<sc_uint <8> > input_row[3];


  //variables
  unsigned char *bitmapData;
  unsigned char  *bitmapFinalImage;
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


  SC_CTOR( tb_sobel){
  
 
    SC_CTHREAD(send,clk.pos());
    reset_signal_is(rst,false);
  
    SC_CTHREAD(recv,clk.pos());
    reset_signal_is(rst,false);
    
  }

  ~tb_sobel(){};

};

#endif  //  TB_SOBEL_H_

  
