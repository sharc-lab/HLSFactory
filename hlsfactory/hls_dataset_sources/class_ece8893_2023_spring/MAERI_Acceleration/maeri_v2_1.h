#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fstream>
#include <assert.h>
#include <iomanip>
#define __gmp_const const
#define AP_INT_MAX_W 4096

#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"
#include "hls_math.h"
#include "hls_streamofblocks.h"

const int RW_SIZE = 2048;
const int MAX_STREAMING_BUF_ENTRY_HALF = 2048;
const int MAX_UB_BUF_ENTRY = 2048;
constexpr int UB_WIDTH                  =   128;     // bandwidth of high-performance interfaces in the design.
constexpr int HP_IFC_BANDWIDTH          =   128;     // bandwidth of high-performance interfaces in the design.
constexpr int URAM_WIDTH                =   72;
constexpr int URAM_HEIGHT               =   4096;
constexpr int MAX_IACT_BUF_ENTRY        =   65536;     // MAX_K*MAX_C*MAX_R*MAX_S/PAR_NUM_WEIGHTS/NUM_HP_IFC = 720*720*3*3 -> 4665600 * 8
constexpr int NUMBER_UB_DEMAND_WRITING  =   32;                                              // H*W = 2048×8 -> 256*64
constexpr int STEP_STATIONARY_BUFF      =   257;
constexpr int STEP_STREAM_BUFF          =   257;


// for the scale
constexpr int BN_COEFF_INT_BITWIDTH        =   8;
constexpr int BN_COEFF_POINT_BITWIDTH      =   10;
constexpr int BN_COEFF_FIX_POINT_BITWIDTH  =   18;

constexpr int IACTS_DATAWIDTH       = 8;
constexpr int OACTS_DATAWIDTH       = 8;
constexpr int WORDS_PER_ADDR        = 16;
constexpr int NUM_ALIGNED_BUFFER    = 4;


ap_uint<8> mult_18x18_o8_1(
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_a,
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_b
);

ap_uint<8> mult_18x18_o8_2(
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_a,
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_b
);

ap_uint<8> mult_18x18_o8_3(
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_a,
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_b
);


////////////////////////////////////////////////////////////////////////////////////////////////////////
// AFFT related:
// Common Functionality Definition
#define LOG_1(n) (((n) >= 2) ? 1 : 0)
#define LOG_2(n) (((n) >= 1<<2) ? (2 + LOG_1((n)>>2)) : LOG_1(n))
#define LOG_4(n) (((n) >= 1<<4) ? (4 + LOG_2((n)>>4)) : LOG_2(n))
#define LOG_8(n) (((n) >= 1<<8) ? (8 + LOG_4((n)>>8)) : LOG_4(n))
#define LOG(n)   (((n) >= 1<<16) ? (16 + LOG_8((n)>>16)) : LOG_8(n))


// Data Width Parameters.
constexpr int AFFT_DATAWIDTH                     = IACTS_DATAWIDTH;
constexpr int NUM_INPUT_DATA                     = 16;
constexpr int NUM_HALF_STAGE                     = LOG(NUM_INPUT_DATA);
constexpr int NUM_STAGE                          = (NUM_HALF_STAGE << 1) - 1;

// Control Signals Parameters 
constexpr int NUM_CTRL_BITS                      = 3*8;

// Switch Control Definition.
constexpr int THROUGH                           = 0b000;
constexpr int SWITCH                            = 0b001;
constexpr int MAX0                              = 0b010;
constexpr int MAX1                              = 0b011;
constexpr int MAX_THROUGH                       = 0b100;
constexpr int MAX_TO_RES_THROUGH                = 0b101;
constexpr int MAX_FROM_RES_THROUGH              = 0b110;

constexpr int NO_RESIDUE                        = 0b000;
constexpr int TO_RESIDUE                        = 0b001;
constexpr int WITH_RESIDUE                      = 0b010;


void afft_cmp8(
    hls::stream<ap_uint<AFFT_DATAWIDTH> >        data_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<OACTS_DATAWIDTH> >        output_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<NUM_CTRL_BITS> >         control_stream[NUM_STAGE]
);

void afft_cmp16(
    hls::stream<ap_uint<AFFT_DATAWIDTH> >       data_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<OACTS_DATAWIDTH> >      output_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<NUM_CTRL_BITS> >        control_stream[NUM_STAGE],
    int                                         afft_run_cycles
);


void feed_data_to_afft(
    ap_uint<HP_IFC_BANDWIDTH>               buff_iacts[RW_SIZE],
    ap_uint<HP_IFC_BANDWIDTH>               buff_rafft_ctrl_reg[RW_SIZE],
    hls::stream<ap_uint<IACTS_DATAWIDTH> >  afft_input_data_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<NUM_CTRL_BITS> >    afft_input_ctrl_stream[NUM_STAGE],
    int                                     read_addr_from_buff_start,
    int                                     read_addr_from_buff_end,
    int                                     num_ifc_entry_iacts,
    int                                     kernel_size,
    int                                     maxpool_w_out,
    int                                     maxpool_h_out,
    int                                     num_oacts,
    int                                     stage_4_seq_cntr,
    bool                                    enable_output_stage_cntr,
    bool                                    afft_routing_reset,
    int                                     afft_routing_limit
);

void maeri_v2_1(
    ap_uint<HP_IFC_BANDWIDTH>   *ifc1,
    ap_uint<HP_IFC_BANDWIDTH>   *ifc2,
    ap_uint<HP_IFC_BANDWIDTH>   *ifc3,
    ap_uint<HP_IFC_BANDWIDTH>   *ifc4,
    ap_uint<HP_IFC_BANDWIDTH>   *ifc5,
    ap_uint<HP_IFC_BANDWIDTH>   *ifc6,
    int                         num_ifc_entry_iacts,
    int                         iacts_zp

);

void get_data_from_afft(
    hls::stream<ap_uint<OACTS_DATAWIDTH> >  afft_output_data_stream[NUM_INPUT_DATA],
    ap_uint<HP_IFC_BANDWIDTH>               buff_in2,
    int                                     write_addr_from_buff_start,
    int                                     write_addr_from_buff_end,
    int                                     num_ifc_entry_iacts,
    int                                     maxpool_w_out,
    int                                     maxpool_h_out,
    int                                     num_oacts,
    int                                     stage_4_seq_cntr,
    int                                     enable_output_stage_cntr,
    bool                                    afft_routing_reset,
    int                                     afft_routing_limit
);