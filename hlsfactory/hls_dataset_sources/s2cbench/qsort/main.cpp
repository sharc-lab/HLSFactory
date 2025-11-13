//========================================================================================
// 
//
// File Name    : main.cpp
// Description  : Testbench for quick sort
// Release Date : 01/08/2013
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra
//
// Revision History
//---------------------------------------------------------------------------------------
// Date        Version      Author                 Description
//----------------------------------------------------------------------------------------
//01/08/2013    1.0         PolyU  DARC Lab      Quick sort main module
//
// g++ -o qsort.exe main.cpp tb_qsort.cpp qsort.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//=======================================================================================


#include "qsort.h"
#include "tb_qsort.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;
 
   sc_signal<sc_uint<8> > indata;
   sc_signal<sc_uint<8> > odata;

   quicksort u_QSORT("QSORT");
   test_qsort u_test("test_QSORT");

   //connect to bubble sort
   u_QSORT.clk( clk );
   u_QSORT.rst( rst );

   u_QSORT.indata( indata);
   u_QSORT.odata(  odata );
 
  // connect to test bench
  u_test.clk( clk );
  u_test.rst( rst );
  
  u_test.indata( indata );
  u_test.odata( odata );


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
   sc_trace(trace_file, indata          , "indata"); 
   sc_trace(trace_file,  odata            ,"odata");
 
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
