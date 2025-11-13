//========================================================================================
// 
// File Name    : main.cpp
// Description  : disparity top system description
// Release Date : 23/02/2018
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Jianqi Chen, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 23/02/2018    1.0           DARClab          Top system declaration
//
// g++ -o disparity.exe main.cpp tb_disparity.cpp disparity.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm
//========================================================================================
#include "define.h"
#include "disparity.h"
#include "tb_disparity.h"

int sc_main(int argc, char** argv)
{
	sc_clock                  clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
	sc_signal<bool>           rst;
	sc_signal<sc_uint<8> >    data_in_r;
	sc_signal<sc_uint<8> >    data_in_l;
	sc_signal<sc_uint<16> >   width;
	sc_signal<sc_uint<8> >    data_out;
	sc_signal<bool>           ready;
	
	int i;
	
	disparity u_disparity("disparity");
	test_disparity test("test_disparity");
	
	// connect to disparity
	u_disparity.clk( clk );
	u_disparity.rst( rst );
	u_disparity.data_in_r( data_in_r );
	u_disparity.data_in_l( data_in_l );

	u_disparity.width( width );
	u_disparity.data_out( data_out );
	u_disparity.ready( ready );
	
	// connect to test bench
	test.clk( clk );
	test.rst( rst );
	test.data_out( data_out );
	test.ready( ready );
	test.data_in_r( data_in_r );
	test.data_in_l( data_in_l );
	test.image_width( width );

	sc_start( 25, SC_NS );
	rst.write(0);

	sc_start( 25, SC_NS );
	rst.write(1);

	sc_start();
	
	return 0;
	
};