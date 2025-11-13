
//========================================================================================
// 
// File Name    : tb_uart.h
// Description  : Testbench
// Release Date : 02/12/2014
// Author       : PolyU DARC Lab
//                Benjamin Carrion Schafer, Nandeesh Veeranna
// 
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version    Author      Description
//---------------------------------------------------------------------------------------
//14/02/2013      1.0       PolyU    UART original testbench
//=======================================================================================


#include <systemc.h>
SC_MODULE (tb_uart)
{
    //inputs

    sc_in<bool> clk;
    sc_in<bool> reset;
    sc_out<bool> rx;
    sc_out<bool> t_ack;
    sc_out<sc_uint<8> > t_data;


    //outputs

    sc_in<sc_uint<8> > r_data;
    sc_in<bool> r_ack;
    sc_in<bool> tx;


    //File pointers

    FILE *transmit_file;
    FILE *receive_file,*diff_file,*receive_file_read;

   /*  transmit function  */
    void uart_transmit(void);

   /*  Receive function  */
    void uart_receive(void);

   /*  Compare function  */
    void compare_data_receive(void);

   /* Constructor   */

    SC_CTOR(tb_uart)
    {
       
        SC_CTHREAD(uart_transmit,clk.pos());
        reset_signal_is(reset,true);
        
        SC_CTHREAD(uart_receive,clk.pos());
        reset_signal_is(reset,true);
    };

    ~tb_uart(){}

};
