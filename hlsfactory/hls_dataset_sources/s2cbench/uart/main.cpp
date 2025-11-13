
//========================================================================================
// 
// File Name    : main.cpp
// Description  : Testbench
// Release Date : 02/12/2014
// Author       : PolyU DARClab
//                Benjamin Carrion Schafer, Nandeesh Veeranna
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      		      Description
//---------------------------------------------------------------------------------------
// 02/12/2014      1.0     Nandeesh Veeranna, PolyU    UART Top System Decleration
//=======================================================================================

// g++ -o uart.exe main.cpp tb_uart.cpp uart.cpp -I$SYSTEMC_HOME/include -L$SYSTEMC_HOME/lib -lsystemc -lm


#include "uart.h"
#include "tb_uart.h"
int sc_main (int argc, char* argv[])
{
    //  sc_clock clk ("my_clock",1,0.5);
    sc_clock clk("clk", 25, SC_NS, 0.5, 12.5, SC_NS, true);
    sc_signal<bool> reset;
    sc_signal<bool> rx;
    sc_signal<bool> t_ack;
    sc_signal<sc_uint<8> > t_data;
    sc_signal<sc_uint<8> > r_data;
    sc_signal<bool> r_ack;
    sc_signal<bool> tx;

    uart u_uart("UART");
    tb_uart test("Test_uart");

    //connect to DUT

    u_uart.clk(clk);
    u_uart.reset(reset);
    u_uart.rx(rx);
    u_uart.t_ack(t_ack);
    u_uart.t_data(t_data);
    u_uart.r_data(r_data);
    u_uart.r_ack(r_ack);
    u_uart.tx(tx);

    //connect to test_bench
    test.clk(clk);
    test.reset(reset);
    test.rx(rx);
    test.t_ack(t_ack);
    test.t_data(t_data);
    test.r_data(r_data);
    test.r_ack(r_ack);
    test.tx(tx);

//VCD file generation
#ifdef WAVE_DUMP
    sc_trace_file *wf = sc_create_vcd_trace_file("uart");
    sc_trace(wf,clk,"clk");
    sc_trace(wf,reset,"reset");
    sc_trace(wf,rx,"rx");
    sc_trace(wf,t_ack,"t_ack");
    sc_trace(wf,t_data,"t_data");
    sc_trace(wf,r_data,"r_data");
    sc_trace(wf,r_ack,"r_ack");
    sc_trace(wf,tx,"tx");
#endif

//Start of Simulation
    reset.write(1);
    sc_start( 25, SC_NS );
    reset.write(0);
    rx = 1;
    sc_start();

#ifdef WAVE_DUMP
    sc_close_vcd_trace_file(wf);
#endif

    return 0;
};
