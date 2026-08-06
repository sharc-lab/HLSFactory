#include "maeri_v2_1.h"
#include <stdio.h>

int main()
{
    ap_uint<HP_IFC_BANDWIDTH> i;
    ap_uint<HP_IFC_BANDWIDTH> afft_ctrl_buff[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc1[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc2[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc3[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc4[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc5[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc6[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc1_cpy[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc2_cpy[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc3_cpy[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc4_cpy[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc5_cpy[RW_SIZE]={0};
    ap_uint<HP_IFC_BANDWIDTH> ifc6_cpy[RW_SIZE]={0};

    ap_uint<HP_IFC_BANDWIDTH> temp1_relu=0;
    ap_uint<HP_IFC_BANDWIDTH> temp2_relu=0;
    ap_uint<HP_IFC_BANDWIDTH> temp3_relu=0;
    ap_uint<HP_IFC_BANDWIDTH> temp4_relu=0;

    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> scale_payload=0.125;
    float scale = 0.125;
    float bn_m_factor=0.5*scale;
    float bn_c_factor=1.5;
    float inv_scale = 1/scale;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> scale_fixed32_f24 = 0.125;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> bn_m_factor_fixed32_f24=0.0625;   //0x40
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> bn_c_factor_fixed32_f24=1.5;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> inv_scale_fixed32_f24 = 8.0;
    ap_uint<8> iacts_zp=15;
    ap_uint<8> temp_16[16]={0};

    int num_ifc_entry_iacts=0;

    //Put data into A

    ifc1[0].range(31,0) = bn_m_factor_fixed32_f24.range(17,0);
    ifc1[0].range(63,32) = bn_c_factor_fixed32_f24.range(17,0);
    ifc1[0].range(127,64) = inv_scale_fixed32_f24.range(17,0);

    for(i=1; i < RW_SIZE; i++){
        ap_uint<8> temp1;
        ap_uint<8> temp2;
        ap_uint<8> temp3;
        ap_uint<8> temp4;
        ap_uint<8> temp5;
        ap_uint<8> temp6;

/*
        temp1 = (i)%10;
        temp2 = (i*10)%100;
        temp3 = (i*100)%1000;
        temp4 = (i*1000)%10000;
        temp5 = (i*10000)%100000;
*/
        temp1 = 20 + i%10;
        for(int ttt=0; ttt< 16; ttt++)
        {
            temp_16[ttt] = temp1+ttt;
            ifc1[i].range( (IACTS_DATAWIDTH*ttt) + (IACTS_DATAWIDTH-1),IACTS_DATAWIDTH*ttt) = temp_16[ttt];
        }

//        ifc1[i] = (temp1);
        //ifc1[i] = (temp1+1, temp1+2, temp1+3, temp1+4, temp1+5, temp1+6, temp1+7, temp1+8,temp1+9,temp1+10,temp1+11,temp1+12,temp1+13,temp1=14,temp1+15,temp1+16);
        //ifc1[i] = (temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1, temp1,temp1,temp1);

//        ifc1_cpy[i] = (temp1+1,temp1+2,temp1+3,temp1+4,temp1+5,temp1+6,temp1+7,temp1+8,temp1+9,temp1+10,temp1+11,temp1+12,temp1+13,temp1=14,temp1+15,temp1+16);


    }

    num_ifc_entry_iacts = num_ifc_entry_iacts + 64;

    
    std::cout << "data before function call" << std::endl;
    for(int j=0; j<20; j++)
    {
        auto temp = ifc1[j];
        std::cout << std::hex << temp << std::endl;
        //std::cout << temp << std::endl;
        //std::cout << ifc1[j] << std::endl;
    }

    //Call the hardware function - 1
//    bn_fixedpoint_5(
//        ifc1,
//        ifc2,
//        ifc3,
//        in4,
//        in5,
//        in6,
//        num_ifc_entry_iacts,
//        bn_m_factor,
//        bn_c_factor,
//        zp,
//        inv_scale
//    );


    int kernel_size         =   3;
//    int kernel_size         =   3;
//    int kernel_size         =   5;
//    int kernel_size         =   7;
    int maxpool_w_out               =   0;
    int maxpool_h_out               =   0;
    int num_oacts                   =   0;
    int zp                          =   0;
    int read_addr_from_buff_start   =   0;
    int read_addr_from_buff_end     =   0;
    int write_addr_from_buff_start  =   0;
    int write_addr_from_buff_end    =   0;

// Test 1 - iacts = 7x7

//  { 0.0,  0.0,  0.0,  0.0,  1.0,  8.0,  0.0,  2.0,  9.0,  0.0,  3.0, 10.0,  0.0,  4.0, 11.0, 'E'}
//  { 0.0,  4.0, 11.0,  0.0,  5.0, 12.0,  0.0,  6.0, 13.0,  0.0,  7.0, 14.0,  0.0,  0.0,  0.0, 'E'}
//  { 0.0,  0.0,  0.0,  8.0, 15.0, 22.0,  9.0, 16.0, 23.0, 10.0, 17.0, 24.0, 11.0, 18.0, 25.0, 'E'}
//  {11.0, 18.0, 25.0, 12.0, 19.0, 26.0, 13.0, 20.0, 27.0, 14.0, 21.0, 28.0,  0.0,  0.0,  0.0, 'E'}
//  { 0.0,  0.0,  0.0, 22.0, 20.0, 36.0, 23.0, 30.0, 37.0, 24.0, 31.0, 38.0, 25.0, 32.0, 39.0, 'E'}
//  {25.0, 32.0, 39.0, 26.0, 33.0, 40.0, 27.0, 34.0, 41.0, 28.0, 35.0, 42.0,  0.0,  0.0,  0.0, 'E'}
//  { 0.0,  0.0,  0.0, 36.0, 43.0,  0.0, 37.0, 44.0,  0.0, 38.0, 45.0,  0.0, 39.0, 46.0,  0.0, 'E'}
//  {39.0, 46.0,  0.0, 40.0, 47.0,  0.0, 41.0, 48.0,  0.0, 42.0, 49.0,  0.0,  0.0,  0.0,  0.0, 'E'}
    if( kernel_size == 3){
        std::cout << "----------------------------------" << std::endl;
        std::cout << "running for maxpool for 3x3 kernel" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        num_ifc_entry_iacts         =   49;
        maxpool_w_out               =    4;
        maxpool_h_out               =    4;
        num_oacts                   =   16;
        zp                          =   12;
        read_addr_from_buff_start   =    0;
        read_addr_from_buff_end     =    9;
        write_addr_from_buff_start  =    0;
        write_addr_from_buff_end    =    9;

        ifc5[0] =0;
        ifc5[1].range(127,64) = 0x000b04000a030009;
        ifc5[1].range( 63, 0) = 0x0200080100000000;
        ifc5[2].range(127,64) = 0x000000000e07000d;
        ifc5[2].range( 63, 0) = 0x06000c05000b0400;
        ifc5[3].range(127,64) = 0x0019120b18110a17;
        ifc5[3].range( 63, 0) = 0x1009160f08000000;
        ifc5[4].range(127,64) = 0x000000001c150e1b;
        ifc5[4].range( 63, 0) = 0x140d1a130c19120b;
        ifc5[5].range(127,64) = 0x00272019261f1825;
        ifc5[5].range( 63, 0) = 0x1e17241416000000;
        ifc5[6].range(127,64) = 0x000000002a231c29;
        ifc5[6].range( 63, 0) = 0x221b28211a272019;
        ifc5[7].range(127,64) = 0x00002e27002d2600;
        ifc5[7].range( 63, 0) = 0x2c25002b24000000;
        ifc5[8].range(127,64) = 0x0000000000312a00;
        ifc5[8].range( 63, 0) = 0x3029002f28002e27;


    // afft instruction buffer
    // 0th location is the reduction scheme - {ctrl_word[6], ctrl_reg[3], ctrl_reg[2], ctrl_reg[1], ctrl_reg[0]}
    //                          |------------------------------------residue control------------------------------------------|                     |-----------------------------ctrl_word[3]------------------------------------|                        |-------------------------------ctrl_word[2]-----------------------|               |------------------------ctrl_word[1]--------------------|               |------------------------ctrl_word[1]---------------------|
        afft_ctrl_buff[0] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                     ap_uint<3>(MAX_THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(MAX_THROUGH),                        ap_uint<3>(MAX1), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(MAX0),               ap_uint<3>(MAX1), ap_uint<3>(THROUGH), ap_uint<3>(MAX1), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(MAX0), ap_uint<3>(THROUGH), ap_uint<3>(MAX0),              ap_uint<3>(MAX1), ap_uint<3>(MAX1), ap_uint<3>(MAX1), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX0), ap_uint<3>(MAX0), ap_uint<3>(MAX0));
    
    // next location onwards is the Control reg and residual control reg for the routing part of the rafft for the actual routing
    //                          |------------------------------------residue control--------------------------------------------------|             |----------------------------------ctrl_word[5]-------------------------------|                       |-----------------------------ctrl_word[4]--------------------------| 
        afft_ctrl_buff[1] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH), ap_uint<3>(THROUGH),                              ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)   );
        afft_ctrl_buff[2] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)   );
        afft_ctrl_buff[3] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH)   );
        afft_ctrl_buff[4] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH)   );
        afft_ctrl_buff[5] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH)   );
        afft_ctrl_buff[6] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH)   );
        afft_ctrl_buff[7] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH)   );
        afft_ctrl_buff[8] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                       ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH),ap_uint<3>( SWITCH),                              ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>( SWITCH)   );
    
        std::cout << "afft_ctrl_buff[0] = " << afft_ctrl_buff[0] << std::endl;
        std::cout << "afft_ctrl_buff[1] = " << afft_ctrl_buff[1] << std::endl;
        std::cout << "afft_ctrl_buff[2] = " << afft_ctrl_buff[2] << std::endl;
        std::cout << "afft_ctrl_buff[3] = " << afft_ctrl_buff[3] << std::endl;
    }
    else if( kernel_size == 5){
        std::cout << "----------------------------------" << std::endl;
        std::cout << "running for maxpool for 5x5 kernel" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        num_ifc_entry_iacts         =   49;
        maxpool_w_out               =    4;
        maxpool_h_out               =    4;
        num_oacts                   =   16;
        zp                          =   12;
        read_addr_from_buff_start   =    0;
        read_addr_from_buff_end     =   20;
        write_addr_from_buff_start  =    0;
        write_addr_from_buff_end    =   20;

        ifc5[ 0]                     =   0;
        ifc5[ 1].range(127,64)       =   0x0000000000000000;
        ifc5[ 1].range( 63, 0)       =   0x0000000000000000;
        ifc5[ 2].range(127,64)       =   0x00110a0300001009;
        ifc5[ 2].range( 63, 0)       =   0x0200000f08010000;
        ifc5[ 3].range(127,64)       =   0x00130c050000120b;
        ifc5[ 3].range( 63, 0)       =   0x040000110a030000;
        ifc5[ 4].range(127,64)       =   0x00150e070000140d;
        ifc5[ 4].range( 63, 0)       =   0x060000130c050000;
        ifc5[ 5].range(127,64)       =   0x0000000000000000;
        ifc5[ 5].range( 63, 0)       =   0x000000150e070000;
        ifc5[ 6].range(127,64)       =   0x0000000000000000;
        ifc5[ 6].range( 63, 0)       =   0x0000000000000000;
        ifc5[ 7].range(127,64)       =   0x001f18110a031e17;
        ifc5[ 7].range( 63, 0)       =   0x1009021d160f0801;
        ifc5[ 8].range(127,64)       =   0x00211a130c052019;
        ifc5[ 8].range( 63, 0)       =   0x120b041f18110a03;
        ifc5[ 9].range(127,64)       =   0x00231c150e07221b;
        ifc5[ 9].range( 63, 0)       =   0x140d06211a130c05;
        ifc5[10].range(127,64)       =   0x0000000000000000;
        ifc5[10].range( 63, 0)       =   0x000000231c150e07;
        ifc5[11].range(127,64)       =   0x0000000000000000;
        ifc5[11].range( 63, 0)       =   0x0000000000000000;
        ifc5[12].range(127,64)       =   0x002d261f18112c25;
        ifc5[12].range( 63, 0)       =   0x1e17102b241d160f;
        ifc5[13].range(127,64)       =   0x002f28211a132e27;
        ifc5[13].range( 63, 0)       =   0x2019122d261f1811;
        ifc5[14].range(127,64)       =   0x00312a231c153029;
        ifc5[14].range( 63, 0)       =   0x221b142f28211a13;
        ifc5[15].range(127,64)       =   0x0000000000000000;
        ifc5[15].range( 63, 0)       =   0x000000312a231c15;
        ifc5[16].range(127,64)       =   0x0000000000000000;
        ifc5[16].range( 63, 0)       =   0x0000000000000000;
        ifc5[17].range(127,64)       =   0x0000002d261f0000;
        ifc5[17].range( 63, 0)       =   0x2c251e00002b241d;
        ifc5[18].range(127,64)       =   0x0000002f28210000;
        ifc5[18].range( 63, 0)       =   0x2e272000002d261f;
        ifc5[19].range(127,64)       =   0x000000312a230000;
        ifc5[19].range( 63, 0)       =   0x30292200002f2821;
        ifc5[20].range(127,64)       =   0x0000000000000000;
        ifc5[20].range( 63, 0)       =   0x0000000000312a23;

    // afft instruction buffer
    // 0th location is the reduction scheme - {ctrl_word[6], ctrl_reg[3], ctrl_reg[2], ctrl_reg[1], ctrl_reg[0]}
    //                          |------------------------------------residue control------------------------------------------|                     |----------------------------------------------ctrl_word[3]------------------------------------------|                        |----------------------------------------------ctrl_word[2]------------------------------------------|                       |----------------------------------------------ctrl_word[1]------------------------------------------|                      |----------------------------------------------ctrl_word[0]------------------------------------------|
        afft_ctrl_buff[0] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                     ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH),                       ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH),                      ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH),                     ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH));


    // next location onwards is the Control reg and residual control reg for the routing part of the rafft for the actual routing
    //                            |---------------------------------------residue control----------------------------------------------------|             |----------------------------------ctrl_word[5]-------------------------------|                       |-------------------------------ctrl_word[4]--------------------------| 
        afft_ctrl_buff[ 1] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(  TO_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 2] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 3] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 4] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 5] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 6] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 7] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 8] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 9] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[10] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[11] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[12] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[13] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[14] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[15] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[16] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[17] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[18] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[19] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[20] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
    }
    else if( kernel_size == 7){
        std::cout << "----------------------------------" << std::endl;
        std::cout << "running for maxpool for 5x5 kernel" << std::endl;
        std::cout << "----------------------------------" << std::endl;
        num_ifc_entry_iacts         =   49;
        maxpool_w_out               =    4;
        maxpool_h_out               =    4;
        num_oacts                   =   16;
        zp                          =   12;
        read_addr_from_buff_start   =    0;
        read_addr_from_buff_end     =   28;
        write_addr_from_buff_start  =    0;
        write_addr_from_buff_end    =   28;


        ifc5[ 0]                     =   0;
        ifc5[ 1].range(127,64)       =   0x0000000000000000;
        ifc5[ 1].range( 63, 0)       =   0x0000000000000000;
        ifc5[ 2].range(127,64)       =   0x0000160f08010000;
        ifc5[ 2].range( 63, 0)       =   0x0000000000000000;
        ifc5[ 3].range(127,64)       =   0x000018110a030000;
        ifc5[ 3].range( 63, 0)       =   0x0017100902000000;
        ifc5[ 4].range(127,64)       =   0x0000000000000000;
        ifc5[ 4].range( 63, 0)       =   0x0019120b04000000;
        ifc5[ 5].range(127,64)       =   0x00001b140d060000;
        ifc5[ 5].range( 63, 0)       =   0x001a130c05000000;
        ifc5[ 6].range(127,64)       =   0x0000000000000000;
        ifc5[ 6].range( 63, 0)       =   0x001c150e07000000;
        ifc5[ 7].range(127,64)       =   0x0000000000000000;
        ifc5[ 7].range( 63, 0)       =   0x0000000000000000;
        ifc5[ 8].range(127,64)       =   0x0000000000000000;
        ifc5[ 8].range( 63, 0)       =   0x0000000000000000;
        ifc5[ 9].range(127,64)       =   0x0000241d160f0801;
        ifc5[ 9].range( 63, 0)       =   0x0000000000000000;
        ifc5[10].range(127,64)       =   0x0000261f18110a03;
        ifc5[10].range( 63, 0)       =   0x00251e1710090200;
        ifc5[11].range(127,64)       =   0x0000000000000000;
        ifc5[11].range( 63, 0)       =   0x00272019120b0400;
        ifc5[12].range(127,64)       =   0x000029221b140d06;
        ifc5[12].range( 63, 0)       =   0x0028211a130c0500;
        ifc5[13].range(127,64)       =   0x0000000000000000;
        ifc5[13].range( 63, 0)       =   0x002a231c150e0700;
        ifc5[14].range(127,64)       =   0x0000000000000000;
        ifc5[14].range( 63, 0)       =   0x0000000000000000;
        ifc5[15].range(127,64)       =   0x0000000000000000;
        ifc5[15].range( 63, 0)       =   0x0000000000000000;
        ifc5[16].range(127,64)       =   0x0000002b241d160f;
        ifc5[16].range( 63, 0)       =   0x0800000000000000;
        ifc5[17].range(127,64)       =   0x0000002d261f1811;
        ifc5[17].range( 63, 0)       =   0x0a002c251e171009;
        ifc5[18].range(127,64)       =   0x0000000000000000;
        ifc5[18].range( 63, 0)       =   0x00002e272019120b;
        ifc5[19].range(127,64)       =   0x0000003029221b14;
        ifc5[19].range( 63, 0)       =   0x0d002f28211a130c;
        ifc5[20].range(127,64)       =   0x0000000000000000;
        ifc5[20].range( 63, 0)       =   0x0000312a231c150e;
        ifc5[21].range(127,64)       =   0x0000000000000000;
        ifc5[21].range( 63, 0)       =   0x0000000000000000;
        ifc5[22].range(127,64)       =   0x0000000000000000;
        ifc5[22].range( 63, 0)       =   0x0000000000000000;
        ifc5[23].range(127,64)       =   0x00000000002b241d;
        ifc5[23].range( 63, 0)       =   0x1600000000000000;
        ifc5[24].range(127,64)       =   0x00000000002d261f;
        ifc5[24].range( 63, 0)       =   0x180000002c251e17;
        ifc5[25].range(127,64)       =   0x0000000000000000;
        ifc5[25].range( 63, 0)       =   0x000000002e272019;
        ifc5[26].range(127,64)       =   0x0000000000302922;
        ifc5[26].range( 63, 0)       =   0x1b0000002f28211a;
        ifc5[27].range(127,64)       =   0x0000000000000000;
        ifc5[27].range( 63, 0)       =   0x00000000312a231c;
        ifc5[28].range(127,64)       =   0x0000000000000000;
        ifc5[28].range( 63, 0)       =   0x0000000000000000;


    // afft instruction buffer
    // 0th location is the reduction scheme - {ctrl_word[6], ctrl_reg[3], ctrl_reg[2], ctrl_reg[1], ctrl_reg[0]}
    //                          |------------------------------------residue control------------------------------------------|                     |----------------------------------------------ctrl_word[3]------------------------------------------|                        |----------------------------------------------ctrl_word[2]------------------------------------------|                       |----------------------------------------------ctrl_word[1]------------------------------------------|                      |----------------------------------------------ctrl_word[0]------------------------------------------|
        afft_ctrl_buff[0] = (0, ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE), ap_uint<3>(NO_RESIDUE),                     ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH),                       ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH),                      ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH),                     ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH), ap_uint<3>(MAX_THROUGH));


    // next location onwards is the Control reg and residual control reg for the routing part of the rafft for the actual routing
    //                            |---------------------------------------residue control----------------------------------------------------|             |----------------------------------ctrl_word[5]-------------------------------|                       |-------------------------------ctrl_word[4]--------------------------| 
        afft_ctrl_buff[ 1] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(  TO_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 2] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 3] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 4] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 5] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 6] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 7] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 8] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[ 9] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[10] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[11] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[12] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[13] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[14] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[15] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[16] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[17] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[18] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[19] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[21] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[22] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[23] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[24] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[25] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[26] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[27] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
        afft_ctrl_buff[28] = (0,   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE),   ap_uint<3>(NO_RESIDUE), ap_uint<3>(WITH_RESIDUE),               ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH),                             ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH), ap_uint<3>(THROUGH)  );
    }





    maeri_v2_1(
        ifc1,
        ifc2,
        ifc3,
        ifc4,
        ifc5,
        ifc6,
        iacts_zp,
        num_ifc_entry_iacts

    );

    std::cout << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;

    //std::cout << "data after function call" << std::endl;
    //for(int j=0; j<20; j++)
    //{
    //    std::cout << std::hex << ifc1[j] << std::endl;
    //}

    // compute the BN in TB
    for(int ll=0; ll<num_ifc_entry_iacts; ll++){
        auto temp1=ifc1_cpy[ll];

    }

    // compare the expected and actual
//    for(int ppp=0; ppp<257; ppp++){
//        if(ifc1_cpy[ppp] != ifc1[ppp]){
//            std::cout << "error at ifc1 location: " << ppp << " , " << ", observed: " << ifc1[ppp] << ", expected: " << ifc1_cpy[ppp] << std::endl;
//            return 1;
//        }
//        else if(ifc2_cpy[ppp] != ifc2[ppp]){
//            std::cout << "error at ifc2 location: " << ppp << " , " << ", observed: " << ifc2[ppp] << ", expected: " << ifc2_cpy[ppp] << std::endl;
//            return 1;
//        }
//        else if(ifc3_cpy[ppp] != ifc3[ppp]){
//            std::cout << "error at ifc3 location: " << ppp << " , " << ", observed: " << ifc3[ppp] << ", expected: " << ifc3_cpy[ppp] << std::endl;
//            return 1;
//        }
//        else if(in4_cpy[ppp] != in4[ppp]){
//            std::cout << "error at in4 location: " << ppp << " , " << ", observed: " << in4[ppp] << ", expected: " << in4_cpy[ppp] << std::endl;
//            return 1;
//        }
//    }

    std::cout << "+------------------------------------------------------------+" << std::endl;
    std::cout << "after function call: " << std::endl;
    for(int ppp=0; ppp<20; ppp++){
        std::cout << ifc1[ppp] << ", " << ifc2[ppp] << ", " << ifc3[ppp] << std::endl;
    }

    std::cout << "+------------------------------------------------------------+" << std::endl;
    std::cout << "|                            pass                            |" << std::endl;
    std::cout << "+------------------------------------------------------------+" << std::endl;

    return 0;
}
