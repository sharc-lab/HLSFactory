//========================================================================================
// 
// File Name    : jpeg_decoder.cpp
// Description  : JPEG decoder main functionality 
// Release Date : 21/07/2017
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 21/07/2017   1.0         PolyU, UTD DARClab  JPEG decoder testbench
//========================================================================================

#include "jpeg_decoder.h"
//#include "decoder.cpp"

/*
 * Description: Initialization in reset phase
 */
void decoder::init()
{
    decoding_done.write(0);

    for (int i = 0; i < P_OUT; i++)
        odata[i].write(0);

    // for (int i = 0; i < MEM_SIZE; i++)
    // {
    //     data_in[i] = 0;
    //     data_out[i] = 0;
    // }

    width = 0;
    height = 0;
    data_size = 0;
}

/*
 * Description: Input ports operation
 */
void decoder::recv()
{
    width = image_width.read().to_int();
    height = image_height.read().to_int();
    data_size = image_size.read().to_int();

    wait();

#ifdef DEBUG
    cout << sc_time_stamp() << " [decoder] Start receving" << endl;
#endif

    for (int i = 0; i < MEM_SIZE; i+=P_IN)
    {
        for (int j = 0; j < P_IN; j++)
            data_in[i + j] = idata[j].read();

        wait();

        if (i >= data_size - P_IN)
            break;
    }

#ifdef DEBUG
    cout << sc_time_stamp() << " [decoder] Recving done" << endl;
#endif
}

/*
 * Description: Output ports operation
 */
void decoder::send()
{
#ifdef DEBUG
    cout << sc_time_stamp() << " [decoder] Sending outputs" << endl;
#endif
    decoding_done.write(1);

    for (int i = 0; i < MEM_SIZE; i+=P_OUT)
    {
        for (int j = 0; j < P_OUT; j++)
            odata[j].write(data_out[i + j]);

        wait();

        if (i >= width * height - P_OUT)
            break;
    }

    decoding_done.write(0);

#ifdef DEBUG
    cout << sc_time_stamp() << " [decoder] Sending done" << endl;
#endif

}

/*
 * Description: Data processing
 */
void decoder::exec()
{
#ifdef DEBUG
    cout << sc_time_stamp() << " [decoder] Start execution" << endl;
#endif

    decode_block(data_in, data_size, width, height, data_out);

#ifdef DEBUG
    cout << sc_time_stamp() << " [decoder] Execution done" << endl;
#endif
}

void decoder::run()
{
    init();

    wait();

    while(true)
    {
        recv();

        exec();

        send();
    }
}
