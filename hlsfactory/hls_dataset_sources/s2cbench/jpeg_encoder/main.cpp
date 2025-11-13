//========================================================================================
// 
// File Name    : main.cpp
// Description  : JPEG testbench 
// Release Date : 30/05/2017
// Author       : UT Dallas DARClab
//                Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 30/05/2017        1.0        UTD DARClab     JPEG encoder SystemC main
//=======================================================================================

#include "jpeg_encoder.h"
#include "tb_jpeg_encoder.h"

int sc_main(int argc, char* argv[]){

  sc_clock                clk("clk", 25, SC_NS, 0.5, 0, SC_NS, true);
  sc_signal<bool> rst;

  sc_signal<sc_uint<8> > input_col[8];
  sc_signal<sc_uint<1> > output[64];
  sc_signal<sc_uint<9> > last_bit;
  int i;
  char name[15];
  

  jpeg_encoder s_obj("jpeg_encoder");
  tb_jpeg_encoder tb_obj("tb_jpeg_encoder");
  s_obj.clk(clk);
  s_obj.rst(rst);


  
  for(i=0;i< 8; i++){
    s_obj.input_col[i](input_col[i]);
  }
  for(i=0;i<64;i++){
    s_obj.output[i](output[i]);
  }
  s_obj.last(last_bit);

  

  tb_obj.clk(clk);
  tb_obj.rst(rst);

  for(i=0;i< 8; i++){
    tb_obj.input_col[i](input_col[i]);
  }
  for(i=0;i<64;i++){
    tb_obj.output[i](output[i]);
  }
  tb_obj.last_bit(last_bit);




#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
 for(i=0; i < SIZE_BUFFER; i++){
   sprintf(name, "input_row%d",i);   
   sc_trace(trace_file, input_row[i]     , name);
 }
 
 sc_trace(trace_file, output_row         , "output_row");
  

#endif  // End WAVE_DUMP

 
   sc_start(25, SC_NS );
   rst.write(0);
  
   sc_start(25, SC_NS );
   rst.write(1);


   sc_start();

#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif


  return 0;
}
