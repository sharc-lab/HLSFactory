//========================================================================================
// 
// File Name    : image.h
// Description  : hw image declaration
// Release Date : 10/02/2015
// Author       : PolyU DARClab
//                Siyuan Xu
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author              Description
//---------------------------------------------------------------------------------------
//10/02/2015      1.1     PolyU DARClab      main component declaration of hw_image
//=======================================================================================
#ifndef IMAGE_H_
#define IMAGE_H_

#include <systemc.h>

SC_MODULE(image)
{
   sc_in_clk sys_clk;
   sc_in<bool> reset_n;
   sc_in<bool >  disp_ena; //display enable ('1' = display time, '0' = blanking time)
   sc_in<int  > column; // row pixel coordinate
   sc_in<int > row;  //column pixel coordinate
   sc_out<sc_uint<8> > red; //red magnitude output to DAC
   sc_out<sc_uint<8> > green; //green magnitude output to DAC
   sc_out<sc_uint<8> > blue; //blue magnitude output to DAC

/* S */
    void image_main(void);

 SC_CTOR (image) {
 	SC_CTHREAD ( image_main, sys_clk.pos()  );
     	  reset_signal_is(reset_n, false);
	  sensitive << disp_ena << column << row;
 	  };
 ~image(){};

};
#endif  // IMAGE_H_

