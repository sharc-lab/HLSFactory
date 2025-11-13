//========================================================================================
// 
// File Name    : jpeg_decoder.h
// Description  : JPEG decoder  
// Release Date : 21/07/2017
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 21/07/2017   1.0         PolyU, UTD DARClab  JPEG decoder module
//=======================================================================================

#ifndef DECODER_H_
#define DECODER_H_

#include "systemc.h"
#include "decoder.h"


SC_MODULE(decoder)
{
    sc_in_clk clk;
    sc_in<bool> rst;

    sc_in<sc_uint<8 > > idata[P_IN];
    sc_in<sc_uint<32> > image_size;
    sc_in<sc_uint<16> > image_width;
    sc_in<sc_uint<16> > image_height;

    sc_out<sc_uint<8> > odata[P_OUT];
    sc_out<sc_uint<1> > decoding_done;

    // Local variables
    unsigned char data_in[MEM_SIZE];
    unsigned char data_out[MEM_SIZE];
    // Image properties
    int width;
    int height;
    int data_size;

    // Functions
    void init();
    void recv();
    void exec();
    void send();

    void run();

    SC_CTOR(decoder)
    {
        SC_CTHREAD(run, clk.pos());
        reset_signal_is(rst, true);
    }

    ~decoder() {}
};

#endif /* DECODER_H_ */
