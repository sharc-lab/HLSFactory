//========================================================================================
// 
// File Name    : tb_jpeg_decoder.cpp
// Description  : JPEG decoder testbench 
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
//=======================================================================================

#include "tb_jpeg_decoder.h"

void tb_decoder::source()
{
    // Initialization
    data_size = 0;
    width = 0;
    height = 0;
    init_sig = 0;

    unsigned char *jpg_data = NULL;

    read_jpeg(&jpg_data, &data_size, &width, &height, "test_encoded.jpg");

    for (int i = 0; i < P_IN; i++)
        pixels[i].write(0);
    image_size.write(0);
    image_width.write(0);
    image_height.write(0);

    rst.write(1);
    wait();

    init_sig = 1;
    image_size.write(data_size);
    image_width.write(width);
    image_height.write(height);
    
    rst.write(0);
    wait();

    cout << sc_time_stamp() << " [tb_decoder] Sourcing data" << endl;
  
  // Send data to decoder
    for (int i = 0; i < MEM_SIZE; i+=P_IN)
    {
        for (int j = 0; j < P_IN; j++)
            pixels[j].write(jpg_data[i + j]);
        wait();

        if (i >= data_size - P_IN)
            break;
    }

    cout << sc_time_stamp() << " [tb_decoder] Sourcing done" << endl;
}

void tb_decoder::sink()
{
    do {
        wait();
    } while(!init_sig);

    unsigned char *image = (unsigned char *)malloc(width * height);
    unsigned char *image_reversed = (unsigned char *)malloc(width * height);

    do {
        wait();
    } while(!decoding_done.read()); // wait until decoding complete

    cout << sc_time_stamp() << " [tb_decoder] Sinking data" << endl;
    // Receive data from decoder
    for (int i = 0; i < MEM_SIZE; i+=P_OUT)
    {
        for (int j = 0; j < P_OUT; j++)
            image[i + j] = image_data[j].read();
        wait();

        if (i >= width * height - P_OUT)
            break;
    }

    cout << sc_time_stamp() << " [tb_decoder] Sinking decoded data done" << endl;

    // Generate JPG image
    int index_old = 0;
    int index_new = 0;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            index_old = i * width + j;
            index_new = (height - 1 - i) * width + j;

            image_reversed[index_new] = image[index_old];
        }
    }

    BmpFileHeader *file_header = (BmpFileHeader *)malloc(sizeof(BmpFileHeader));
    BmpInfoHeader *info_header = (BmpInfoHeader *)malloc(sizeof(BmpInfoHeader));
    ColorTable4 palette[NUM_OF_COLORS];

    unsigned int offset = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader) + sizeof(palette);
    unsigned int size = width * height + offset;

    create_file_header(file_header, size, offset);
    create_info_header(info_header, width, height, 8);
    create_palette(palette);

    write_to_bmp("test_decoded.bmp", file_header, info_header, palette, image_reversed);

    cout << sc_time_stamp() << " [tb_decoder] Output image generated" << endl;

    sc_stop();
}
