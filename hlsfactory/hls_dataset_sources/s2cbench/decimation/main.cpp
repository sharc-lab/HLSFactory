//===========================================================================
//
//
// File Name    : main.cpp
// Description  : Decimation filter testbench
// Release Date : 31/07/2013 
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra 
//
// Revision History
//---------------------------------------------------------------------------
// Date             Author      Version     Description
//---------------------------------------------------------------------------
// 31/07/2013        PolyU          1.0     Top decimation filter system description
//
// g++ -o decim.exe main.cpp tb_decim.cpp fil_decim.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//===========================================================================

#include "filt_decim.h"
#include "tb_decim.h"


int sc_main(int argc, char** argv)
{
  sc_clock                clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
  sc_signal<bool>         rst;

   sc_signal<bool> load_coeff;  
   sc_signal < sc_fixed<19,2,SC_RND,SC_SAT> > indata;

   sc_signal< sc_fixed<17,2,SC_RND,SC_SAT> > incoef1[TAPS_STAGE1] ;
   sc_signal< sc_fixed<17,2,SC_RND,SC_SAT> > incoef2[TAPS_STAGE2] ;
   sc_signal< sc_fixed<17,2,SC_RND,SC_SAT> > incoef3[TAPS_STAGE3] ;
   sc_signal< sc_fixed<17,2,SC_RND,SC_SAT> > incoef4[TAPS_STAGE4] ;
   sc_signal< sc_fixed<17,2,SC_RND,SC_SAT> > incoef5[TAPS_STAGE5] ;
 
   sc_signal < sc_fixed<19, 2, SC_RND, SC_SAT> > odata;

  int x;
  char name[10];


  decfilt u_DECIM("DECIMATION");
  test_decim u_test("test_DECIMATION");

  //connect decimation filter to testbench
  u_DECIM.clk( clk );
  u_DECIM.rst( rst );

  u_DECIM.load_coeff ( load_coeff );
  u_DECIM.indata ( indata );

  for(x=0; x < TAPS_STAGE1; x++){
    u_DECIM.incoef1[x]( incoef1[x] );
  }

  for(x=0; x < TAPS_STAGE2; x++){
    u_DECIM.incoef2[x]( incoef2[x] );
  }

  for(x=0; x < TAPS_STAGE3; x++){
    u_DECIM.incoef3[x]( incoef3[x] );
  }

  for(x=0; x < TAPS_STAGE4; x++){
    u_DECIM.incoef4[x]( incoef4[x] );
  }


  for(x=0; x < TAPS_STAGE5; x++){
    u_DECIM.incoef5[x]( incoef5[x] );
  }

  u_DECIM.odata( odata );



  // connect to testbench
  u_test.clk( clk );
  u_test.rst( rst );
  
  u_test.load_coeff ( load_coeff );
  u_test.indata ( indata );

  for(x=0; x < TAPS_STAGE1; x++){
    u_test.incoef1[x]( incoef1[x] );
  }

  for(x=0; x < TAPS_STAGE2; x++){
    u_test.incoef2[x]( incoef2[x] );
   }

  for(x=0; x < TAPS_STAGE3; x++){
    u_test.incoef3[x]( incoef3[x] );
  }

  for(x=0; x < TAPS_STAGE4; x++){
    u_test.incoef4[x]( incoef4[x] );
  }

  for(x=0; x < TAPS_STAGE5; x++){
    u_test.incoef5[x]( incoef5[x] );
  }

  u_test.odata( odata );


#ifdef WAVE_DUMP
  // Trace files
  sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

  // Top level signals
  sc_trace(trace_file, clk              , "clk");
  sc_trace(trace_file, rst              , "rst");
 
  sc_trace(trace_file, load_coeff        , "load_coeff");
  sc_trace(trace_file, indata             , "indata");

 for(x=0; x < TAPS_STAGE1; x++){
   sprintf(name, "in_coeff1%d",x);   
   sc_trace(trace_file, incoef1[x]          , name);
 }

 for(x=0; x < TAPS_STAGE2; x++){
   sprintf(name, "in_coeff2%d",x);   
   sc_trace(trace_file, incoef2[x]          , name);
 }

 for(x=0; x < TAPS_STAGE3; x++){
   sprintf(name, "in_coeff3%d",x);   
   sc_trace(trace_file, incoef3[x]          , name);
 }

 for(x=0; x < TAPS_STAGE4; x++){
   sprintf(name, "in_coeff4%d",x);   
   sc_trace(trace_file, incoef4[x]          , name);
 }

 for(x=0; x < TAPS_STAGE5; x++){
   sprintf(name, "in_coeff5%d",x);   
   sc_trace(trace_file, incoef5[x]          , name);
 }

  sc_trace(trace_file, odata    , "odata");

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
