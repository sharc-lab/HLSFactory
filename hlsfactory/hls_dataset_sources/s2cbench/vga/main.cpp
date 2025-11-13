//===========================================================================
//
// File Name    : main.cpp
// Description  : vga top system description
// Release Date : 10/02/2015
// Author       : PolyU DARC Lab
//                Siyuan Xu
//
// Revision History
//---------------------------------------------------------------------------
// Date            Version   Author    Description
//---------------------------------------------------------------------------
//10/02/2015         1.1      PolyU     Top system declaration
//
// g++ -o vga.exe main.cpp tb_fir.cpp fir.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "top_vga.h"
#include "tb_vga.h"


int sc_main(int argc, char** argv)
{
  sc_clock                sys_clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         reset_n;

   sc_signal< bool >  h_sync ;//--horiztonal sync pulse
   sc_signal< bool >  v_sync ;//--vertical sync pulse
   sc_signal< bool >   n_blank  ;//--direct blacking output to DAC
   sc_signal< bool >   n_sync  ;//--sync-on-green output to DAC
   sc_signal <sc_uint<8> > red; //red magnitude output to DAC
   sc_signal <sc_uint<8> > green; //green magnitude output to DAC
   sc_signal<sc_uint<8> > blue; //blue magnitude output to DAC

   top_vga u_top("top_vga");
   test_top test("test_top");

  //connect to FIR
  u_top.sys_clk( sys_clk );
  u_top.reset_n( reset_n );
  u_top.h_sync(h_sync) ;//--horiztonal sync pulse
  u_top.v_sync(v_sync) ;//--vertical sync pulse
  u_top.n_blank(n_blank)  ;//--direct blacking output to DAC
  u_top.n_sync(n_sync)  ;//--sync-on-green output to DAC
  u_top.red(red); //red magnitude output to DAC
  u_top.green(green); //green magnitude output to DAC
  u_top.blue(blue); //blue magnitude output to DAC


  // connect to test bench
  test.sys_clk( sys_clk );
  test.reset_n( reset_n );
  test.h_sync(h_sync) ;//--horiztonal sync pulse
  test.v_sync(v_sync) ;//--vertical sync pulse
  test.n_blank(n_blank)  ;//--direct blacking output to DAC
  test.n_sync(n_sync)  ;//--sync-on-green output to DAC
  test.red(red); //red magnitude output to DAC
  test.green(green); //green magnitude output to DAC
  test.blue(blue); //blue magnitude output to DAC


#ifdef WAVE_DUMP

  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, sys_clk   , "sys_clk");
  sc_trace(trace_file, reset_n  , "reset_n");
  sc_trace(trace_file, h_sync    , "h_sync");
  sc_trace(trace_file, v_sync    , "v_sync");
  sc_trace(trace_file, n_blank   , "n_blank");
  sc_trace(trace_file, n_sync    , "n_sync");
  sc_trace(trace_file, red    ,     "red");
  sc_trace(trace_file, blue    , "blue");
  sc_trace(trace_file, green   , "green");

#endif  // End WAVE_DUMP

  sc_start( 25, SC_NS );
  reset_n.write(0);

  sc_start( 25, SC_NS );
  reset_n.write(1);

  sc_start();


#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif

    return 0;

};
