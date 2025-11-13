#include "define.h"
#include "aes.h"

const U8 sbox[256] = {
    //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
};

const U8 rcon[Nr] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36};


void aes::assign_key(U8 key_in[SIZE], U8 key_out[SIZE])
{
    for (int i = 0; i < SIZE; i++)
        key_out[i] = key_in[i];
}

void aes::SubBytes(U8 state[SIZE])
{
    for (int i = 0; i < SIZE; i++)
        state[i] = sbox[state[i]];
}

void aes::ShiftRows(U8 state[SIZE])
{
    for (int row = 0; row < 4; row++)
    {
        for (int counter = 0; counter < row; counter++)
        {
            int row_ind = Nb * row;
            U8 tmp = state[row_ind];
            int col = 0;
			
            for (col = 1; col < Nb; col++)
            {
                state[row_ind + col - 1] = state[row_ind + col];
            }
            state[row_ind + col - 1] = tmp;
        }
    }
}

void aes::MixColumns(U8 state[SIZE])
{
    U8 state_tmp[4][Nb];
    U8 col_tmp[4];
    U8 x_2, x_3, x_1;

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < Nb; col++)
            state_tmp[row][col] = state[row * Nb + col];

    for (int col = 0; col < Nb; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            int indice[4];
            for (int i = 0; i < 4; i++)
            {
                indice[i] = clip(row + i);
            }
            
            x_2 = xtime_2(state_tmp[indice[0]][col]);
            x_3 = xtime_3(state_tmp[indice[1]][col]);
            x_1 = xadd(state_tmp[indice[2]][col], state_tmp[indice[3]][col]);
            col_tmp[row] = xadd(xadd(x_2, x_3), x_1);
        }

        for (int row = 0; row < 4; row++)
            state_tmp[row][col] = col_tmp[row];
    }

    for (int row = 0; row < 4; row++)
    {
        for (int col = 0; col < Nb; col++)
        {
            state[row * Nb + col] = state_tmp[row][col];
        }
    }
}

U8 aes::xtime_2(U8 data)
{
    U8 tmp = data << 1;
    if (data >= 0x80)
        tmp = tmp ^ 0x1b;
    return tmp;
}

U8 aes::xadd(U8 in1, U8 in2)
{
    return (in1 ^ in2);
}

U8 aes::xtime_3(U8 data)
{
    U8 x_2 = xtime_2(data);
    return xadd(x_2, data);
}

int aes::clip(int n)
{
    if (n > 3)
        n = n - 4;
    return n;
}

void aes::KeyExpansion(U8 current_key[SIZE], U8 rcon, U8 next_key[SIZE])
{
    U8 key_in_tmp[4][Nb];
    U8 key_out_tmp[4][Nb];

    for (int i = 0; i < 4; i++)
        for (int j = 0; j < Nb; j++)
            key_in_tmp[i][j] = current_key[i * Nb + j];

    for (int col = 0; col < Nb; col++)
    {
        U8 col_tmp[4];
        if (col == 0)
        {
            col_tmp[0] = sbox[key_in_tmp[1][Nb - 1]] ^ rcon;
            col_tmp[1] = sbox[key_in_tmp[2][Nb - 1]];
            col_tmp[2] = sbox[key_in_tmp[3][Nb - 1]];
            col_tmp[3] = sbox[key_in_tmp[0][Nb - 1]];
        }
        else
        {
            for (int i = 0; i < 4; i++)
                col_tmp[i] = key_out_tmp[i][col - 1];
        }
        for (int row = 0; row < 4; row++)
            key_out_tmp[row][col] = key_in_tmp[row][col] ^ col_tmp[row];
    }

    for (int row = 0; row < 4; row++)
        for (int col = 0; col < Nb; col++)
            next_key[row * Nb + col] = key_out_tmp[row][col];
}

void aes::AddRoundKey(U8 state[SIZE], U8 key[SIZE])
{
    for (int i = 0; i < SIZE; i++)
        state[i] = state[i] ^ key[i];
}

void aes::run()
{
    U8 idata_buf[SIZE];
    U8 key_buf[SIZE];
    U8 current_key[SIZE];
    U8 next_key[SIZE];
    
    wait();

    while(1)
    {
        for (int i = 0; i < SIZE; i++)
        {
            idata_buf[i] = idata[i].read();
            key_buf[i] = ikey[i].read();
        }

        assign_key(key_buf, current_key);

        // Initial round
        AddRoundKey(idata_buf, current_key);

        // 9 rounds
        for (int i = 0; i < Nr; i++)
        {
            SubBytes(idata_buf);
            ShiftRows(idata_buf);
            if (i < Nr - 1)
                MixColumns(idata_buf);
            KeyExpansion(current_key, rcon[i], next_key);
            AddRoundKey(idata_buf, next_key);

            assign_key(next_key, current_key);
        }

        for (int i = 0; i < SIZE; i++)
            odata[i].write(idata_buf[i]);

        wait();
    }
}
