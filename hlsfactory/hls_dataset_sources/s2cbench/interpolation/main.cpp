//========================================================================================
// 
// File Name    : main.cpp
// Description  : Main interpolation filter Testbench 
// Release Date : 23/02/2018
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Anushree Mahapatra, Jianqi Chen
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version   Author       Description
//----------------------------------------------------------------------------------------
//23/07/2013      1.0       PolyU      main testbench for interpolation filter
//23/02/2018        1.1    UTD        increase number of data outputs to 4
//=======================================================================================

#include "tb_interp.h"
#include "filter_interp.h"

int sc_main(int argc, char* argv[]){

	int i;
	sc_clock clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
	sc_signal<bool> rst;

	sc_signal< sc_fixed<16,2,SC_TRN,SC_WRAP> > indata;
	sc_signal< sc_ufixed<9,0,SC_TRN,SC_WRAP> > infactor;

	sc_signal< bool > odata_en;
	sc_signal< sc_fixed<16,2,SC_RND,SC_SAT> > odata[4];

	interp interp_obj("interp");
	tb_interp interp_tbobj("tb_interp");

	interp_obj.clk(clk);
	interp_tbobj.clk(clk);

	interp_obj.rst(rst);
	interp_tbobj.rst(rst);

	interp_obj.indata(indata);
	for(i=0;i<4;i++)
		interp_obj.odata[i](odata[i]);
	interp_obj.odata_en(odata_en);

	interp_tbobj.indata(indata);
	for(i=0;i<4;i++)
		interp_tbobj.odata[i](odata[i]);
	interp_tbobj.odata_en(odata_en);


#ifdef WAVE_DUMP
	// Trace files
	sc_trace_file* trace_file = sc_create_vcd_trace_file("trace_behav");

	// Top level signals
	sc_trace(trace_file, clk           , "clk");
	sc_trace(trace_file, rst           , "rst");

	sc_trace(trace_file, indata          , "indata");
	sc_trace(trace_file, infactor          , "infactor");

	sc_trace(trace_file,interp_obj.SoP1       ,"SoP1");  
	sc_trace(trace_file,interp_obj.SoP2       ,"SoP2");
	sc_trace(trace_file,interp_obj.SoP3       ,"SoP3");
	sc_trace(trace_file,interp_obj.SoP4       ,"SoP4");

	sc_trace(trace_file, odata[0]           , "odata_a00");
	sc_trace(trace_file, odata[1]           , "odata_a01");
	sc_trace(trace_file, odata[2]           , "odata_a02");
	sc_trace(trace_file, odata[3]           , "odata_a03");
	sc_trace(trace_file, odata_en         ,"odata_en");  

#endif  // End WAVE_DUMP



   cout<<"\n start simulation\n";

  sc_start( 25, SC_NS );
  rst.write(0);

  sc_start( 25, SC_NS );
  rst.write(1);

  sc_start();



#ifdef WAVE_DUMP
   sc_close_vcd_trace_file(trace_file);
#endif


  return 0;
}
