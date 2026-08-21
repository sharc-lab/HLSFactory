#ifndef INCREMENT_H
#define INCREMENT_H

#include <cynw_p2p.h>

SC_MODULE(increment)
{
    sc_in_clk SC_NAMED(clk);
    sc_in<bool> SC_NAMED(rst);
    cynw_p2p<sc_uint<16>>::in SC_NAMED(din);
    cynw_p2p<sc_uint<16>>::out SC_NAMED(dout);

    SC_CTOR(increment)
    {
        SC_CTHREAD(thread_function, clk.pos());
        reset_signal_is(rst, 0);
        din.clk_rst(clk, rst);
        dout.clk_rst(clk, rst);
    }

    void thread_function();
};

#endif
