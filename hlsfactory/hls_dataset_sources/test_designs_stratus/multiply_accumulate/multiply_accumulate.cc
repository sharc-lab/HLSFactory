#include "multiply_accumulate.h"

void multiply_accumulate::thread_function()
{
    {
        HLS_DEFINE_PROTOCOL("reset");
        a.reset();
        b.reset();
        c.reset();
        result.reset();
        wait();
    }

    while (true) {
        const sc_uint<8> a_value = a.get();
        const sc_uint<8> b_value = b.get();
        const sc_uint<16> c_value = c.get();
        result.put(a_value * b_value + c_value);
    }
}
