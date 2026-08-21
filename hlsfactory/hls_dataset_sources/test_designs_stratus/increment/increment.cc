#include "increment.h"

void increment::thread_function()
{
    {
        HLS_DEFINE_PROTOCOL("reset");
        din.reset();
        dout.reset();
        wait();
    }

    while (true) {
        const sc_uint<16> value = din.get();
        dout.put(value + 1);
    }
}
