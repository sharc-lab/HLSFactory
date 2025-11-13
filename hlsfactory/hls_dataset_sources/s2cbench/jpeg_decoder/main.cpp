//========================================================================================
//
// File Name    : main.cpp
// Description  : JPEG decoder 
// Release Date : 21/07/2017
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version     Author             Description
//----------------------------------------------------------------------------------------
// 21/07/2017    1.0         PolyU,UTD DARClab JPEG decoder SystemC main
//=======================================================================================

#include "systemc.h"
#include "jpeg_decoder.h"
#include "tb_jpeg_decoder.h"
#include "common.h"


int sc_main(int argc, char** argv)
{

  sc_clock                clk_sig("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst_sig;
  int x;
  char name[10];

   decoder *u_jpeg_decoder;
   tb_decoder *u_test;

   sc_signal<sc_uint<8 > > idata_sig[P_IN];
   sc_signal<sc_uint<32> > size_sig;
   sc_signal<sc_uint<16> > width_sig;
   sc_signal<sc_uint<16> > height_sig;
   sc_signal<sc_uint<8 > > odata_sig[P_OUT];
   sc_signal<sc_uint<1 > > done_sig;


   u_test = new tb_decoder("test_jpeg_decoder");
   u_test->clk(clk_sig);
   u_test->rst(rst_sig);
   u_test->image_size(size_sig);
   u_test->image_width(width_sig);
   u_test->image_height(height_sig);

   for (int i = 0; i < P_IN; i++)
   	 u_test->pixels[i](idata_sig[i]);
        
   u_test->decoding_done(done_sig);
   for (int i = 0; i < P_OUT; i++)
        u_test->image_data[i](odata_sig[i]);

   u_jpeg_decoder = new decoder("decoder0");	
   u_jpeg_decoder->clk(clk_sig);
   u_jpeg_decoder->rst(rst_sig);
   u_jpeg_decoder->image_size(size_sig);
   u_jpeg_decoder->image_width(width_sig);
   u_jpeg_decoder->image_height(height_sig);
   for (int i = 0; i < P_IN; i++)
           u_jpeg_decoder->idata[i](idata_sig[i]);
     
   u_jpeg_decoder->decoding_done(done_sig);
        for (int i = 0; i < P_OUT; i++)
            u_jpeg_decoder->odata[i](odata_sig[i]);
   


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk_sig            , "clk");
  sc_trace(trace_file, rst_sig            , "rst");
 
   sc_trace(trace_file, size_sig      , "size"); 
   sc_trace(trace_file, width_sig     , "width");
   sc_trace(trace_file, size_sig      , "size"); 
   sc_trace(trace_file, height_sig    , "height");

 for(x=0; x < P_IN; x++){
   sprintf(name, "idata%d",x);   
   sc_trace(trace_file, idata_sig[x]          , name);
}

 for(x=0; x < P_OUT; x++){
   sprintf(name, "odata%d",x);   
   sc_trace(trace_file, odata_sig[x]          , name);
}


#endif  // End WAVE_DUMP

  sc_start();

#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif



}



