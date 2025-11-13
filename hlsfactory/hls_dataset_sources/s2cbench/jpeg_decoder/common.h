//========================================================================================
// 
// File Name    : config.h
// Description  : JPEG decoder configuration file 
// Release Date : 21/07/2017
// Author       : PolyU, UT Dallas DARClab
//                Shuangnan Liu, Benjamin Carrion Schafer
// 
//
// Revision History
//---------------------------------------------------------------------------------------
// Date         Version         Author          Description
//----------------------------------------------------------------------------------------
// 21/07/2017   1.0         PolyU, UTD DARClab  configuration file
//=======================================================================================

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>

#define BLOCK_LEN 8 // compressed block by block
#define BLOCK_SIZE (BLOCK_LEN * BLOCK_LEN)
#define CODE_LEN_MAX 16 // the maximum length of Huffman code for DC/AC
#define MEM_SIZE 1024 * 1024 // maximum image dimension
#define P_IN  64 // number of input ports
#define P_OUT 1 // number of output ports
#define PI 314 //159265358979323846
#define CLIP(X) ( (X) > 255 ? 255 : (X) < 0 ? 0 : X) // [0-255]


int
rad_to_deg(int rad);

int
pow_func(int base, int expo);

void
de_quantization(int *data_in, int *table, int *data_out);

void
idct(int *data_in, int *data_out);

void
data_shift_up(int *data_in, unsigned char *data_out);

void
decode_block(unsigned char *data_in,
             int data_size,
             int width,
             int height,
             unsigned char *data_out);


#ifdef DEBUG
    #define DEBUG_PRINT(x) printf x
#else
    #define DEBUG_PRINT(x) do {} while(0)
#endif

/*
 * Description: print to stdout for debugging
 */

inline void
print_matrix(void *matrix, int width, int height, int mode)
{
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = i * width + j;
            if (mode == 0) // integer
                DEBUG_PRINT(("%4d ", ((int *)matrix)[index]));
            else if (mode == 1) // float
                DEBUG_PRINT(("%7.2f ", ((float *)matrix)[index]));
            else if (mode == 2) // 0x00
                DEBUG_PRINT(("%02x ", ((unsigned char *)matrix)[index]));
            else if (mode == 3) // 255
                DEBUG_PRINT(("%4u ", ((unsigned char *)matrix)[index]));
        }
        DEBUG_PRINT(("\n"));
    }
}

#endif /* COMMON_H_ */

