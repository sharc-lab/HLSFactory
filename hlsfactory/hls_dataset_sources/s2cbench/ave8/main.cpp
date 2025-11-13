//===========================================================================
//
// File Name    : main.cpp
// Description  : ave8 top system description
// Release Date : 12/08/2017
// Author       : DARClab
//
// Revision History
//---------------------------------------------------------------------------
// Date            Version   Author    Description
//---------------------------------------------------------------------------
//12/08/2017         1.0     DARClab     Top system declaration
//
// g++ -o ave8.exe main.cpp tb_ave8.cpp ave8.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "ave8.h"
#include "tb_ave8.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
  sc_signal<sc_uint<8> >  in_data;

 
  sc_signal<sc_uint<8> > ave8_output; 
  int x;
  char name[10];


  ave8 u_AVE8("AVE*");
  test_AVE8 test("test_AVE8");

  //connect to ave8 module
  u_AVE8.clk( clk );
  u_AVE8.rst( rst );


  u_AVE8.in_data( in_data );

  u_AVE8.ave8_output( ave8_output );

  // connect to test bench
  test.clk( clk );
  test.rst( rst );
  

   test.in_data( in_data );


  test.ave8_output( ave8_output );


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst"); 
  sc_trace(trace_file, in_data          , "ind_data");
  sc_trace(trace_file, filter_output    , "ave8_output");

#endif  // End WAVE_DUMP

  sc_start( 25, SC_NS );
  rst.write(0);

  sc_start( 25, SC_NS );
  rst.write(1);

  sc_start();

#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif

    return 0;

};
