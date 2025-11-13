//========================================================================================
// 
// File Name    : decode.cpp
// Description  : main decoder functions
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


#include "decoder.h"

const int zig_zag_table[] =
{
     0,  1,  8, 16,  9,  2,  3, 10,
    17, 24, 32, 25, 18, 11,  4,  5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13,  6,  7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

const int sin_table[] = // 10000 times
{
       0,  175,  349,  523,  698,
     872, 1045, 1219, 1392, 1564,
    1736, 1908, 2079, 2250, 2419,
    2588, 2756, 2924, 3090, 3256,
    3420, 3584, 3746, 3907, 4067,
    4226, 4384, 4540, 4695, 4848,
    5000, 5150, 5299, 5446, 5592,
    5736, 5878, 6018, 6157, 6293,
    6428, 6561, 6691, 6820, 6947,
    7071, 7193, 7314, 7431, 7547,
    7660, 7771, 7880, 7986, 8090,
    8192, 8290, 8387, 8480, 8572,
    8660, 8746, 8829, 8910, 8988,
    9063, 9135, 9205, 9272, 9336,
    9397, 9455, 9511, 9563, 9613,
    9659, 9703, 9744, 9781, 9816,
    9848, 9877, 9903, 9925, 9945,
    9962, 9976, 9986, 9994, 9998,
    10000
};

// Masks for bit selection
const unsigned int masks_high[] =
{
    0x00000000, 0x80000000, 0xC0000000, 0xE0000000,
    0xF0000000, 0xF8000000, 0xFC000000, 0xFE000000,
    0xFF000000, 0xFF800000, 0xFFC00000, 0xFFE00000,
    0xFFF00000, 0xFFF80000, 0xFFFC0000, 0xFFFE0000,
    0xFFFF0000
};

const unsigned int masks_low[] =
{
    0x00000000, 0x00000001, 0x00000003, 0x00000007,
    0x0000000F, 0x0000001F, 0x0000003F, 0x0000007F,
    0x000000FF, 0x000001FF, 0x000003FF, 0x000007FF,
    0x00000FFF, 0x00001FFF, 0x00003FFF, 0x00007FFF,
    0x0000FFFF
};

// The following five tables should be extracted from JPG markers.
// However, for simplicity, these tables are given.
const int dq_dc_t[] =
{
     16,  11,  12,  14,  12,  10,  16,  14, 
     13,  14,  18,  17,  16,  19,  24,  40, 
     26,  24,  22,  22,  24,  59,  35,  37, 
     29,  40,  58,  51,  61,  60,  57,  51, 
     56,  55,  64,  72,  92,  78,  64,  68, 
     87,  69,  55,  56,  80, 109,  81,  87, 
     95,  98, 103, 104, 103,  62,  77, 113, 
    121, 112, 100, 120,  92, 101, 103,  99
};

const unsigned int dc_y_t[][3] =
{
    { 0, 0x00FF, 0x0000},
    { 0, 0x0000, 0x0000},
    { 1, 0x0002, 0x0006},
    { 6, 0x000E, 0x000E},
    { 7, 0x001E, 0x001E},
    { 8, 0x003E, 0x003E},
    { 9, 0x007E, 0x007E},
    {10, 0x00FE, 0x00FE},
    {11, 0x01FE, 0x01FE}
};

const int dc_y_ht[] =
{
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
};

const unsigned int ac_y_t[][3] =
{
    { 0, 0x00FF, 0x0000}, // 1
    { 0, 0x0000, 0x0001}, // 2
    { 2, 0x0004, 0x0004}, // 3
    { 3, 0x000A, 0x000C}, // 4
    { 6, 0x001A, 0x001C}, // 5
    { 9, 0x003A, 0x003B}, // 6
    {11, 0x0078, 0x007B}, // 7
    {15, 0x00F8, 0x00FA}, // 8
    {18, 0x01F6, 0x01FA}, // 9
    {23, 0x03F6, 0x03FA}, // 10
    {28, 0x07F6, 0x07F9}, // 11
    {32, 0x0FF4, 0x0FF7}, // 12
    { 0, 0xFFFF, 0x0000}, // 13
    { 0, 0xFFFF, 0x0000}, // 14
    {36, 0x7FC0, 0x7FC0}, // 15
    {37, 0xFF82, 0xFFFE}  // 16
};

const int ac_y_ht[] =
{
      1,  2,                // 2
      3,                    // 1
      0,  4,  17,           // 3
      5, 18,  33,           // 3
     49, 65,                // 2
      6, 19,  81,  97,      // 4
      7, 34, 113,           // 3
     20, 50, 129, 145, 161, // 5
      8, 35,  66, 177, 193, // 5
     21, 82, 209, 240,      // 4
     36, 51,  98, 114,      // 4
                            // 0
                            // 0
    130,                    // 1

                            // 125
      9,  10,  22,  23,  24,  25,  26,  37,  38,  39,
     40,  41,  42,  52,  53,  54,  55,  56,  57,  58,
     67,  68,  69,  70,  71,  72,  73,  74,  83,  84,
     85,  86,  87,  88,  89,  90,  99, 100, 101, 102,
    103, 104, 105, 106, 115, 116, 117, 118, 119, 120,
    121, 122, 131, 132, 133, 134, 135, 136, 137, 138,
    146, 147, 148, 149, 150, 151, 152, 153, 154, 162,
    163, 164, 165, 166, 167, 168, 169, 170, 178, 179,
    180, 181, 182, 183, 184, 185, 186, 194, 195, 196,
    197, 198, 199, 200, 201, 202, 210, 211, 212, 213,
    214, 215, 216, 217, 218, 225, 226, 227, 228, 229,
    230, 231, 232, 233, 234, 241, 242, 243, 244, 245,
    246, 247, 248, 249, 250        
};

/*
 * Description: de-quantization
 */
void
de_quantization(int *data_in, const int *table, int *data_out)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
        data_out[zig_zag_table[i]] = data_in[i] * table[i];
}

int
cos_func(int rad)
{
    int deg = rad * 180 / PI;
    int deg_t = deg % 90;
    int quadrant = (deg / 90) % 4;
    int cos_val = 0;

    if (quadrant == 0)
        cos_val = sin_table[90 - deg_t];
    else if (quadrant == 1)
        cos_val = -sin_table[deg_t];
    else if (quadrant == 2)
        cos_val = -sin_table[90 - deg_t];
    else
        cos_val = sin_table[deg_t];

    return cos_val;
}

int
pow_func(int base, int expo)
{
    int ret = 1;
    for (int i = 0; i < expo; i++)
        ret *= base;
    return ret;
}

/*
 * Description: IDCT
 */
void
idct(int *data_in, int *data_out)
{
    int a_u = 0;
    int a_v = 0;
    int a_uv = 0;
    int cos_tmp = 0;
    int tmp = 0;
    int index = 0;
    int index_inside = 0;

    for (int x = 0; x < 8; x++)
    {
        for (int y = 0; y < 8; y++)
        {
            index = y * 8 + x;
            tmp = 0;
            for (int u = 0; u < 8; u++)
            {
                for (int v = 0; v < 8; v++)
                {
                    a_v = (v == 0)? 707: 1000;
                    a_u = (u == 0)? 707: 1000;
                    index_inside = v * 8 + u;
                    a_uv = a_v * a_v / 1000;
                    cos_tmp = cos_func((2 * x + 1) * u * PI / 16)
                            * cos_func((2 * y + 1) * v * PI / 16) / 100000;
                    tmp += data_in[index_inside] * a_uv * cos_tmp / 1000;
                }
            }
            data_out[index] = tmp / 4000;
        }
    }
}


/*
 * Description: add 128 to each pixel
 */
void
data_shift_up(int *data_in, unsigned char *data_out)
{
    for (int i = 0; i < BLOCK_SIZE; i++)
        data_out[i] = CLIP(data_in[i] + 128);
}

/*
 * Description: decode the encoded data into blocks of pixel values
 */
void
decode_block(unsigned char *data_in,
             int data_size,
             int width,
             int height,
             unsigned char *data_out)
{
    // Local variables for decoding
    unsigned int data_buf    = 0; // read 32 bits, unsigned for logical shift
    unsigned int data_tmp    = 0; // for shifting
    int init_shift      = 0; // shift if previous code length is not a multiple of 8
    int code_len        = 0; // Huffman code length
    unsigned int code_val    = 0; // code value
    int read_bytes_num  = 4; // number of bytes to be read in one iteration
    int dc_done         = 0; // if DC has been decoded
    unsigned int max_val     = 0; // Max value for decoding
    unsigned int min_val     = 0; // Min value for decoding
    int val_ptr         = 0; // the index of min_val
    int decoded_num     = 0; // the number of decoded pixels
    int index           = 0; // 
    int cat             = 0; // DC category
    // AC Run-Length
    int rrrrssss = 0;
    int rrrr     = 0;
    int ssss     = 0;

    int sign     = 0; // sign for DC/AC
    int dc       = 0;
    int pre_dc   = 0;
    int ac       = 0;

    int last_block = 0; // check if meets the last block
    int block[BLOCK_SIZE] = {0}; // decoded block
    int block_dq[BLOCK_SIZE] = {0}; // after de-quantization
    int block_idct[BLOCK_SIZE] = {0}; // after IDCT
    unsigned char block_restored[BLOCK_SIZE] = {0}; // final block

    int index_block = 0;
    int index_global = 0;
    int block_width = width / BLOCK_LEN;
    int block_id = 0;
    int block_id_v = 0;
    int block_id_h = 0;

    // Start decoding
    for (int i = 0; i < data_size; )
    {
        // Read bit stream
        for (int j = 0; j < read_bytes_num; j++, i++)
        {
            data_buf = (data_buf << 8) + data_in[i];

            // 0xFF00 detection
            if (i < data_size - 1 && data_in[i] == 0xFF && data_in[i + 1] == 0x00)
                i++;
        }

        // Initial shift
        data_tmp = data_buf << init_shift;

#ifdef DEBUG
        DEBUG_PRINT(("Data: %08x, Date_tmp: %08x, Init_shift: %02d\n", data_buf, data_tmp, init_shift));
#endif

        // Decode one DC/AC
        for (int j = 0; j < CODE_LEN_MAX; j++) // read bit-by-bit
        {
            code_len = j + 1; // current code length
            // get the Huffman code value
            code_val = (data_tmp & masks_high[code_len]);
            code_val = code_val >> (32 - code_len);
#ifdef DEBUG
            DEBUG_PRINT(("Code_val: %08x\n", code_val));
#endif
            if (dc_done == 0) //decode DC
            {
                max_val = dc_y_t[j][2];
                min_val = dc_y_t[j][1];

                if (code_val >= min_val && code_val <= max_val) // a valid code
                {
#ifdef DEBUG
                    DEBUG_PRINT(("Code_val: %04x, [%04x, %04x]\n", code_val, min_val, max_val));
#endif
                    // Get category
                    val_ptr = dc_y_t[j][0];
                    index   = val_ptr + (code_val - min_val);
                    cat     = dc_y_ht[index];

                    data_tmp <<= code_len; // left align DC value

                    // Get DC diff
                    sign = data_tmp & masks_high[1]; // if 1, positive
                    dc   = (data_tmp & masks_high[cat]) >> (32 - cat);
                    if (sign == 0) // if negative
                        dc = dc + 1 - pow_func(2, cat);
                        // dc = dc + 1 - pow(2, cat);

                    data_tmp <<= cat; // remove DC code
                    code_len += cat; // update code length

                    dc                 = dc + pre_dc;
                    block[decoded_num] = dc; // store DC in the block
                    pre_dc             = dc;


                    // increase decoded count
                    decoded_num++;
                    dc_done = 1; // finish DC decoding

                    j = CODE_LEN_MAX; // stop the loop
                } // a valid code
            } // decode DC
            else // decode AC
            {
                max_val = ac_y_t[j][2];
                min_val = ac_y_t[j][1];

                if (code_val >= min_val && code_val <= max_val) // a valid code
                {
                    // Get Run-Length
                    val_ptr   = ac_y_t[j][0]; // beginning index
                    index     = val_ptr + (code_val - min_val);
                    rrrrssss  = ac_y_ht[index];
                    ssss      = rrrrssss & masks_low[4]; // length
                    rrrr      = rrrrssss >> 4; // 0 run

                    data_tmp <<= code_len; // left align AC value

                    if (rrrrssss == 0) // EOB
                    {
                        for (; decoded_num < BLOCK_SIZE; decoded_num++)
                            block[decoded_num] = 0;
                    }
                    else // Get AC
                    {
                        sign = data_tmp & masks_high[1]; // if 1, positive
                        ac   = (data_tmp & masks_high[ssss]) >> (32 - ssss);
                        if (sign == 0) // if negative
                            ac = ac + 1 - pow_func(2, ssss);
                            // ac = ac + 1 - pow(2, ssss);

                        data_tmp <<= ssss; // remove AC code
                        code_len += ssss; // update code length

                        // store 0's and AC in the block
                        for (int ii = 0; ii < rrrr; ii++, decoded_num++)
                            block[decoded_num] = 0;

                        block[decoded_num] = ac;
                        decoded_num++;
                    }
                    j = CODE_LEN_MAX; // stop the loop
                } // a valid code
            } // decode AC
        } // decode DC/AC


        // Parameters for next iteration
        if (decoded_num == BLOCK_SIZE) // An entire block has been decoded
        {
            decoded_num = 0; // a new block
            dc_done     = 0; // start from DC

            if (last_block) // the decoding completes
                i = data_size;
#ifdef DEBUG
            print_matrix(block, 8, 8, 0); // output the block
#endif
            de_quantization(block, dq_dc_t, block_dq);

#ifdef DEBUG
            print_matrix(block_dq, 8, 8, 0);
#endif

            idct(block_dq, block_idct);

#ifdef DEBUG
            print_matrix(block_idct, 8, 8, 0);
#endif

            data_shift_up(block_idct, block_restored);

#ifdef DEBUG
            print_matrix(block_restored, 8, 8, 3);
#endif

            block_id_v = block_id / block_width;
            block_id_h = block_id % block_width;

            for (int ii = 0; ii < BLOCK_LEN; ii++)
            {
                for (int jj = 0; jj < BLOCK_LEN; jj++)
                {
                    index_block = ii * BLOCK_LEN + jj;
                    index_global = (block_id_v * BLOCK_LEN + ii) * width
                                 + block_id_h * BLOCK_LEN + jj;

                    data_out[index_global] = block_restored[index_block];
                }
            }

            block_id++;
        }
        else
        {
            if (i == data_size) // complete the final block
            {
                i--;
                last_block = 1;
            }
        }

        code_len        = code_len + init_shift;
        read_bytes_num  = code_len / 8;
        init_shift      = code_len % 8;
        data_tmp        >>= code_len; // right align the remaining code
        data_buf        = data_tmp;
    }
}
