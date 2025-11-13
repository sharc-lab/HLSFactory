//========================================================================================
// 
// File Name    : tb_jpeg_decoder.h
// Description  : JPEG decoder testbench header 
// Release Date : 21/07/2017
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 21/07/2017   1.0         PolyU, UTD DARClab  JPEG decoder testbench
//=======================================================================================

#ifndef TB_DECODER_H_
#define TB_DECODER_H_

#include "systemc.h"
#include "config_tb.h"

SC_MODULE(tb_decoder)
{
    sc_in_clk clk;
    sc_out<bool> rst;

    sc_out<sc_uint<8> > pixels[P_IN];
    sc_out<sc_uint<32> > image_size;
    sc_out<sc_uint<16> > image_width;
    sc_out<sc_uint<16> > image_height;

    sc_in<sc_uint<8> > image_data[P_OUT];
    sc_in<sc_uint<1> > decoding_done;

    int data_size;
    int width;
    int height;
    int init_sig;

    void source();
    void sink();

    SC_CTOR(tb_decoder)
    {
        SC_CTHREAD(source, clk.pos());
        SC_CTHREAD(sink, clk.pos());

        cout << sc_time_stamp() << " [tb_decoder] Constructor called" << endl;
    }

    ~tb_decoder() {}
};

#endif /* TB_DECODER_H_ */

