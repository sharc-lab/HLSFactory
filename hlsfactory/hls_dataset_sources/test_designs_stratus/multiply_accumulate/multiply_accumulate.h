#ifndef MULTIPLY_ACCUMULATE_H
#define MULTIPLY_ACCUMULATE_H

#include <cynw_p2p.h>

SC_MODULE(multiply_accumulate)
{
    sc_in_clk SC_NAMED(clk);
    sc_in<bool> SC_NAMED(rst);
    cynw_p2p<sc_uint<8>>::in SC_NAMED(a);
    cynw_p2p<sc_uint<8>>::in SC_NAMED(b);
    cynw_p2p<sc_uint<16>>::in SC_NAMED(c);
    cynw_p2p<sc_uint<17>>::out SC_NAMED(result);

    SC_CTOR(multiply_accumulate)
    {
        SC_CTHREAD(thread_function, clk.pos());
        reset_signal_is(rst, 0);
        a.clk_rst(clk, rst);
        b.clk_rst(clk, rst);
        c.clk_rst(clk, rst);
        result.clk_rst(clk, rst);
    }

    void thread_function();
};

#endif
