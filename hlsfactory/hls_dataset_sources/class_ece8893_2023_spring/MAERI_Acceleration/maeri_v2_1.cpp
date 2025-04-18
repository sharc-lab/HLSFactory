
#include"maeri_v2_1.h"



#define RELU_ENABLED

// adjust the below macros to change the configuration of the multiplier implementation in BN
#define BN_ENABLED
#define BN_FULL_UNROLL      // Will fully unroll the above mentioned custom multiplier => Highest Resource utilization + Lowest Latency
#define BN_MULT_OP_DSP      // Will not use any of the above custom multipliers. Will use DSPs only
//#define BN_MULT_OP_TYPE_1   // Using mult_18x18_o8_1
//#define BN_MULT_OP_TYPE_2   // Using mult_18x18_o8_2
//#define BN_MULT_OP_TYPE_3   // Using mult_18x18_o8_3
//#define BN_MULT_OP_HYBRID   // Will be a hybrid of BN_MULT_OP_TYPE_1 + BN_FULL_UNROLL + BN_MULT_OP_DSP, where the number of ops are split evenly between DSPs and custom multipliers

#define MAXPOOL_ENABLED


#ifdef RELU_ENABLED
//-----------------------------------------------------------------------------------------------//
//----------------------------------ReLu operator Function begin---------------------------------//
//-----------------------------------------------------------------------------------------------//
ap_uint<HP_IFC_BANDWIDTH> relu_16words_8bit(
    ap_uint<HP_IFC_BANDWIDTH>  data_16words_8bit,
    ap_uint<8> zeropoint
    )
{
    ap_uint<HP_IFC_BANDWIDTH>  data_16words_8bit_relued=0;
    // compute relu on the 128/8 words at the same time
    for(int pp=0; pp<WORDS_PER_ADDR; pp++){
#pragma HLS UNROLL
        if( data_16words_8bit.range(IACTS_DATAWIDTH*pp + (IACTS_DATAWIDTH-1),IACTS_DATAWIDTH*pp) < zeropoint ){
            data_16words_8bit_relued.range(IACTS_DATAWIDTH*pp + (IACTS_DATAWIDTH-1),IACTS_DATAWIDTH*pp) = 0;
        }
        else{
            data_16words_8bit_relued.range(IACTS_DATAWIDTH*pp + (IACTS_DATAWIDTH-1),IACTS_DATAWIDTH*pp) = data_16words_8bit.range(IACTS_DATAWIDTH*pp + 7,IACTS_DATAWIDTH*pp);
        }
    }
    return data_16words_8bit_relued;
}
//-----------------------------------------------------------------------------------------------//
//-----------------------------------ReLu operator Function end----------------------------------//
//-----------------------------------------------------------------------------------------------//
#endif


//-----------------------------------------------------------------------------------------------//
//-------------------------------Custom Multiplier Functions begin-------------------------------//
//-----------------------------------------------------------------------------------------------//

//-----------------------------------------------------------------------------------------------//
// simplest implementation - lantecy = width of in_b = 18
ap_uint<8> mult_18x18_o8_1(
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_a,
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_b
) {

    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> reg_in_a;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc;

    ap_fixed<2*BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH+BN_COEFF_FIX_POINT_BITWIDTH, AP_RND_CONV, AP_SAT> ttt1 = in_a;
    reg_in_a = ttt1<<(BN_COEFF_FIX_POINT_BITWIDTH - BN_COEFF_INT_BITWIDTH); // made as Fraction = 2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH), total width = 2*BN_COEFF_FIX_POINT_BITWIDTH
    partial_sum_acc = 0;

    for(int i=0; i<BN_COEFF_FIX_POINT_BITWIDTH; i++){
#pragma HLS loop_tripcount min=BN_COEFF_FIX_POINT_BITWIDTH max=BN_COEFF_FIX_POINT_BITWIDTH
#pragma HLS PIPELINE II=1
        if(in_b[i] == 1){
            shifted_in_a = reg_in_a << i;
            partial_sum_acc = partial_sum_acc + shifted_in_a;
        }
    }

    ap_uint<8> temp = partial_sum_acc.range(2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH)+8-1, 2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH));

    return temp;
}

//-----------------------------------------------------------------------------------------------//
// 2 level parallelism implementation - lantecy = width of in_b/2 = 18/2 = 9
ap_uint<8> mult_18x18_o8_2(
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_a,
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_b
) {

// to fully unroll this function and achive fastest latency with max resource utilization, ensure pipeline ii=1 pragma at the top level
#pragma HLS inline off
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> reg_in_a;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a_1;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a_2;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_1;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_2;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_final;

    ap_fixed<2*BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH+BN_COEFF_FIX_POINT_BITWIDTH, AP_RND_CONV, AP_SAT> ttt1 = in_a;
    reg_in_a = ttt1<<(BN_COEFF_FIX_POINT_BITWIDTH - BN_COEFF_INT_BITWIDTH); // made as Fraction = 2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH), total width = 2*BN_COEFF_FIX_POINT_BITWIDTH
    partial_sum_acc_1 = 0;
    partial_sum_acc_2 = 0;

    SHIFT_ADD_9CLK:
    for(int i=0; i<9; i++){
#pragma HLS PIPELINE II=1
        if(in_b[i] == 1){
            shifted_in_a_1      =   reg_in_a << i;
            partial_sum_acc_1   =   partial_sum_acc_1 + shifted_in_a_1;
        }
        if(in_b[9 + i] == 1){
            shifted_in_a_2      =   reg_in_a << 9 + i;
            partial_sum_acc_2   =   partial_sum_acc_2 + shifted_in_a_2;
        }
    }
    partial_sum_acc_final = partial_sum_acc_2 + partial_sum_acc_1;

    ap_uint<8> temp = partial_sum_acc_final.range(2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH)+8-1, 2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH));
    return temp;
}

//-----------------------------------------------------------------------------------------------//
// 3 level parallelism implementation - lantecy = width of in_b/3 = 18/3 = 6
ap_uint<8> mult_18x18_o8_3(
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_a,
        ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> in_b
) {

// to fully unroll this function and achive fastest latency with max resource utilization, ensure pipeline ii=1 pragma at the top level
#pragma HLS inline off
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> reg_in_a;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a_1;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a_2;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a_3;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> shifted_in_a_4;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_1;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_2;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_3;
    ap_uint<2*BN_COEFF_FIX_POINT_BITWIDTH> partial_sum_acc_final;

    ap_fixed<2*BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH+BN_COEFF_FIX_POINT_BITWIDTH, AP_RND_CONV, AP_SAT> ttt1 = in_a;
    reg_in_a = ttt1<<(BN_COEFF_FIX_POINT_BITWIDTH - BN_COEFF_INT_BITWIDTH); // made as Fraction = 2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH), total width = 2*BN_COEFF_FIX_POINT_BITWIDTH
    partial_sum_acc_1 = 0;
    partial_sum_acc_2 = 0;
    partial_sum_acc_3 = 0;

    SHIFT_ADD_6CLK:
    for(int i=0; i<6; i++){
#pragma HLS PIPELINE II=1
        if(in_b[i] == 1){
            shifted_in_a_1      =   reg_in_a << i;
            partial_sum_acc_1   =   partial_sum_acc_1 + shifted_in_a_1;
        }
        if(in_b[6 + i] == 1){
            shifted_in_a_2      =   reg_in_a << 6 + i;
            partial_sum_acc_2   =   partial_sum_acc_2 + shifted_in_a_2;
        }
        if(in_b[12 + i] == 1){
            shifted_in_a_3      =   reg_in_a << 12 + i;
            partial_sum_acc_3   =   partial_sum_acc_3 + shifted_in_a_3;
        }
    }
    partial_sum_acc_final = partial_sum_acc_3 + partial_sum_acc_2 + partial_sum_acc_1;

    ap_uint<8> temp = partial_sum_acc_final.range(2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH)+8-1, 2*(BN_COEFF_FIX_POINT_BITWIDTH-BN_COEFF_INT_BITWIDTH));
    return temp;
}

//-----------------------------------------------------------------------------------------------//
//-------------------------------Custom Multiplier Function end----------------------------------//
//-----------------------------------------------------------------------------------------------//

#ifdef BN_ENABLED
//-----------------------------------------------------------------------------------------------//
//------------------------------Batch Normalization Function begin-------------------------------//
//-----------------------------------------------------------------------------------------------//
ap_uint<HP_IFC_BANDWIDTH> bn_fixedpoint_16words_8bit(
    ap_uint<HP_IFC_BANDWIDTH> data_16words_8bit,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> bn_m_factor,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> bn_c_factor,
    ap_uint<IACTS_DATAWIDTH> zp,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> inv_scale)
{
    ap_uint<HP_IFC_BANDWIDTH> data_16words_8bit_post_bn = 0;
    ap_uint<IACTS_DATAWIDTH> data_minus_zp[WORDS_PER_ADDR] = {0};
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> temp_float[WORDS_PER_ADDR] = {0};
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH + IACTS_DATAWIDTH, BN_COEFF_INT_BITWIDTH + IACTS_DATAWIDTH, AP_RND_CONV, AP_SAT> temp_post_bn_1[WORDS_PER_ADDR] = {0};
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> temp_post_bn_2[WORDS_PER_ADDR] = {0};
    ap_uint<IACTS_DATAWIDTH> temp_reg[WORDS_PER_ADDR] = {0};
    ap_uint<IACTS_DATAWIDTH> data_fl2q[WORDS_PER_ADDR] = {0};
    ap_uint<IACTS_DATAWIDTH> data_fl2q_w_zp[WORDS_PER_ADDR] = {0};

#pragma HLS array_partition variable=data_minus_zp  type=complete
#pragma HLS array_partition variable=temp_float     type=complete
#pragma HLS array_partition variable=temp_post_bn_1 type=complete
#pragma HLS array_partition variable=temp_post_bn_2 type=complete
#pragma HLS array_partition variable=temp_reg       type=complete
#pragma HLS array_partition variable=data_fl2q      type=complete
#pragma HLS array_partition variable=data_fl2q_w_zp type=complete


    BN_OP_16UNROLL:
    for (int tt = 0; tt < WORDS_PER_ADDR; tt++){
#pragma HLS UNROLL
        data_minus_zp[tt] = data_16words_8bit.range((IACTS_DATAWIDTH * tt) + (IACTS_DATAWIDTH - 1), IACTS_DATAWIDTH * tt) - zp;
        temp_post_bn_1[tt] = bn_m_factor*data_minus_zp[tt];
        temp_post_bn_2[tt] = temp_post_bn_1[tt] + bn_c_factor;

        #ifdef BN_MULT_OP_DSP
        data_fl2q[tt] = temp_post_bn_2[tt] * inv_scale;
        #endif
        #ifdef BN_MULT_OP_TYPE_1
        data_fl2q[tt] = mult_18x18_o8_1(temp_post_bn_2[tt], inv_scale);
        #endif
        #ifdef BN_MULT_OP_TYPE_2
        data_fl2q[tt] = mult_18x18_o8_2(temp_post_bn_2[tt], inv_scale);
        #endif
        #ifdef BN_MULT_OP_TYPE_3
        data_fl2q[tt] = mult_18x18_o8_3(temp_post_bn_2[tt], inv_scale);
        #endif
        
        #ifdef BN_MULT_OP_HYBRID
        if(tt < WORDS_PER_ADDR/2){
            data_fl2q[tt] = temp_post_bn_2[tt] * inv_scale;
        }
        else{
            data_fl2q[tt] = mult_18x18_o8_1(temp_post_bn_2[tt], inv_scale);
        }
        #endif
        
        data_fl2q_w_zp[tt] = data_fl2q[tt] + zp;
        data_16words_8bit_post_bn.range((IACTS_DATAWIDTH * tt) + (IACTS_DATAWIDTH - 1), IACTS_DATAWIDTH * tt) = data_fl2q_w_zp[tt];

        //&& uncomment to debug ---- comment to synthesize
        //std::cout << "---------------------------------------------------" << std::endl;
        //std::cout << "inv_scale = " << inv_scale << std::endl;
        //std::cout << "data_q_8bit=" << data_16words_8bit.range( (IACTS_DATAWIDTH*tt) + (IACTS_DATAWIDTH-1),IACTS_DATAWIDTH*tt) << ", minus zp=" << data_minus_zp[tt] << ", with_scale_and_factor=" << temp_post_bn_1[tt];
        //std::cout << ", adding c factor=" << temp_post_bn_2[tt] << ", back to q_8bit=" << data_fl2q[tt] << ", adding zp=" << data_fl2q_w_zp[tt] << std::endl;
    }
    return data_16words_8bit_post_bn;
}

void LoadBatchNormParams(
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc1                                               ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc2                                               ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc3                                               ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc4                                               ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc5                                               ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc6                                               ,
    ap_uint<UB_WIDTH>                                                                   bn_buffer[MAX_STREAMING_BUF_ENTRY_HALF]             , // what depth should we set for this???
    int                                                                                 init_ifc_addr_read                                  ,
    int                                                                                 num_ifc_entry_iacts                                  ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_m_factor                                         ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_c_factor                                         ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   inv_scale
){
     //is the below pragma redundant? as we will take care in top file?
    #pragma HLS array_partition variable=bn_buffer type=cyclic factor=4

    for(int i=0; i<10; i++){
#pragma HLS loop_tripcount min=10 max=10
#pragma HLS DEPENDENCE variable=bn_buffer intra false
        bn_buffer[(0*STEP_STREAM_BUFF) + i] = ifc1[i + init_ifc_addr_read];
    }

    ap_fixed<32, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> temp_bn_m_factor =  bn_buffer[init_ifc_addr_read].range(31,0);
    ap_fixed<32, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> temp_bn_c_factor =  bn_buffer[init_ifc_addr_read].range(63,32);
    ap_fixed<32, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> temp_inv_scale   =  bn_buffer[init_ifc_addr_read].range(95,64);
    
    bn_m_factor = temp_bn_m_factor >> 10;
    bn_c_factor = temp_bn_c_factor >> 10;
    inv_scale   = temp_inv_scale   >> 10;



}

void multi_bn_op(
    int                                                                                 num_ifc_entry_iacts                     ,
    ap_uint<UB_WIDTH>                                                                    stationary_buffer[MAX_UB_BUF_ENTRY]           ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_m_factor                                   ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_c_factor                                   ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   inv_scale                                     ,
    ap_uint<IACTS_DATAWIDTH>                                                            zp
)
{
    //are the below two pragma redundant? as we will take care in top file?
    #pragma HLS BIND_STORAGE variable=stationary_buffer type=ram_t2p impl=bram latency=1
    #pragma HLS array_partition variable=stationary_buffer type=cyclic factor=4

    ap_uint<HP_IFC_BANDWIDTH> bn_shift_reg_arr[NUM_ALIGNED_BUFFER][10] = {0};
    ap_uint<HP_IFC_BANDWIDTH> post_bn_outpt[NUM_ALIGNED_BUFFER] = {0};
#pragma HLS array_partition variable = bn_shift_reg_arr type = complete
#pragma HLS array_partition variable = post_bn_outpt type = complete


    BN_OP_4TIMES:
    for (int i = 0; i < 4 * num_ifc_entry_iacts; i = i + 4){
#pragma HLS loop_tripcount min=257 max=257
#pragma HLS DEPENDENCE variable=stationary_buffer inter false
#ifdef BN_FULL_UNROLL
#pragma HLS PIPELINE II = 1
#endif
        bn_shift_reg_arr[0][0] = stationary_buffer[i + 0];
        bn_shift_reg_arr[1][0] = stationary_buffer[i + 1];
        bn_shift_reg_arr[2][0] = stationary_buffer[i + 2];
        bn_shift_reg_arr[3][0] = stationary_buffer[i + 3];
        if (i > 0){
            post_bn_outpt[0] = bn_fixedpoint_16words_8bit(bn_shift_reg_arr[0][1],
                                                            bn_m_factor,
                                                            bn_c_factor,
                                                            zp,
                                                            inv_scale);
            post_bn_outpt[1] = bn_fixedpoint_16words_8bit(bn_shift_reg_arr[1][1],
                                                            bn_m_factor,
                                                            bn_c_factor,
                                                            zp,
                                                            inv_scale);
            post_bn_outpt[2] = bn_fixedpoint_16words_8bit(bn_shift_reg_arr[2][1],
                                                            bn_m_factor,
                                                            bn_c_factor,
                                                            zp,
                                                            inv_scale);
            post_bn_outpt[3] = bn_fixedpoint_16words_8bit(bn_shift_reg_arr[3][1],
                                                            bn_m_factor,
                                                            bn_c_factor,
                                                            zp,
                                                            inv_scale);
            stationary_buffer[i - 4 + 0] = post_bn_outpt[0];
            stationary_buffer[i - 4 + 1] = post_bn_outpt[1];
            stationary_buffer[i - 4 + 2] = post_bn_outpt[2];
            stationary_buffer[i - 4 + 3] = post_bn_outpt[3];
        }
        bn_shift_reg_arr[0][1] = bn_shift_reg_arr[0][0];
        bn_shift_reg_arr[1][1] = bn_shift_reg_arr[1][0];
        bn_shift_reg_arr[2][1] = bn_shift_reg_arr[2][0];
        bn_shift_reg_arr[3][1] = bn_shift_reg_arr[3][0];
    }
}

void BNCall( 
    int                                                                                 num_ifc_entry_iacts                     ,
    ap_uint<UB_WIDTH>                                                                   stationary_buffer[MAX_UB_BUF_ENTRY]           ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc1                                         ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc2                                         ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc3                                         ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc4                                         ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc5                                         ,
    ap_uint<HP_IFC_BANDWIDTH>                                                           *ifc6                                         ,
    ap_uint<UB_WIDTH>                                                                   bn_buffer[MAX_STREAMING_BUF_ENTRY_HALF]       , // what depth should we set for this???
    int                                                                                 init_ifc_addr_read                            ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_m_factor                                   ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_c_factor                                   ,
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   inv_scale                                     ,
    ap_uint<IACTS_DATAWIDTH>                                                            zp
)
{
    #pragma HLS array_partition variable=bn_buffer type=cyclic factor=4

    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_m_factor1;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   bn_c_factor1;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT>   inv_scale1  ;

    for(int i=0; i<10; i++){
#pragma HLS loop_tripcount min=10 max=10
#pragma HLS DEPENDENCE variable=bn_buffer intra false
        bn_buffer[(0*STEP_STREAM_BUFF) + i] = ifc1[i + init_ifc_addr_read];
    }

    ap_fixed<32, 22, AP_RND_CONV, AP_SAT> temp_bn_m_factor =  bn_buffer[init_ifc_addr_read].range(31,0);
    ap_fixed<32, 22, AP_RND_CONV, AP_SAT> temp_bn_c_factor =  bn_buffer[init_ifc_addr_read].range(63,32);
    ap_fixed<32, 22, AP_RND_CONV, AP_SAT> temp_inv_scale   =  bn_buffer[init_ifc_addr_read].range(95,64);
    
    bn_m_factor1 = temp_bn_m_factor >> 10;
    bn_c_factor1 = temp_bn_c_factor >> 10;
    inv_scale1   = temp_inv_scale   >> 10;



    //&& uncomment to debug ---- comment to synthesize
    //std::cout   << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ " << std::endl;
    //std::cout   <<  "  bn_m_factor = " << bn_m_factor1
    //            <<  ", bn_c_factor = " << bn_c_factor1
    //            <<  ", inv_scale = "   << inv_scale1  
    //            <<  ", zp = "   << zp  
    //            << std::endl;

    BN_OP:
    multi_bn_op(    num_ifc_entry_iacts,
                    stationary_buffer,
                   bn_m_factor1,
                   bn_c_factor1,
                   inv_scale1  ,
                    zp
                    );

    bn_m_factor  = bn_m_factor1;
    bn_c_factor  = bn_c_factor1;
    inv_scale    = inv_scale1  ;

}
//-----------------------------------------------------------------------------------------------//
//------------------------------Batch Normalization Function end---------------------------------//
//-----------------------------------------------------------------------------------------------//
#endif



#ifdef MAXPOOL_ENABLED

void afft_switch(
    ap_uint<AFFT_DATAWIDTH>   in_data_0,
    ap_uint<AFFT_DATAWIDTH>   in_data_1,
    ap_uint<AFFT_DATAWIDTH>   &out_data_0,
    ap_uint<AFFT_DATAWIDTH>   &out_data_1,
    ap_uint<3>                control=0b100 // should be 3 bits?
){
    
    ap_uint<AFFT_DATAWIDTH>   max_data;
    max_data = (in_data_0 > in_data_1)? in_data_0: in_data_1;

    if (control == SWITCH){
        out_data_0 = in_data_1;
        out_data_1 = in_data_0;
    }
    else if (control == MAX0){
        out_data_0 = max_data;
        out_data_1 = in_data_1;
    }
    else if (control == MAX1){
        out_data_0 = out_data_0;
        out_data_1 = max_data;
    }
    else if (control == MAX_THROUGH){
        out_data_0 = max_data;
        out_data_1 = max_data;
    }
    else if(control == THROUGH){ // THROUGH, by default
        out_data_0 = in_data_0;
        out_data_1 = in_data_1;
    }
};



void rafft_switch(
    ap_uint<AFFT_DATAWIDTH>   in_data_0,
    ap_uint<AFFT_DATAWIDTH>   in_data_1,
    ap_uint<AFFT_DATAWIDTH>   &out_data_0,
    ap_uint<AFFT_DATAWIDTH>   &out_data_1,
    ap_uint<AFFT_DATAWIDTH>   &r_data_0,
    ap_uint<AFFT_DATAWIDTH>   &r_data_1,
    ap_uint<AFFT_DATAWIDTH>   &r_max_data_both,
    ap_uint<3>                control=0b000,
    ap_uint<3>                residue_control=0b000
){
    
    ap_uint<AFFT_DATAWIDTH>   max_data;
    ap_uint<AFFT_DATAWIDTH>   r_max_data_0;
    ap_uint<AFFT_DATAWIDTH>   r_max_data_1;

    max_data            = (in_data_0 > in_data_1)? in_data_0: in_data_1;
    r_max_data_0        = ( r_data_0 > in_data_0)?  r_data_0: in_data_0;
    r_max_data_1        = ( r_data_1 > in_data_1)?  r_data_1: in_data_1;
    r_max_data_both     = ( r_data_0 >  r_data_1)?  r_data_0:  r_data_1;


    if(residue_control==NO_RESIDUE){
        if (control == SWITCH){
            out_data_0 = in_data_1;
            out_data_1 = in_data_0;
        }
        else if (control == MAX0){ 
            out_data_0 = max_data;
            out_data_1 = in_data_1;
        }
        else if (control == MAX1){
            out_data_0 = out_data_0;
            out_data_1 = max_data;
        }
        else if (control == MAX_THROUGH){
            out_data_0 = max_data;
            out_data_1 = max_data;
        }
        else if(control == THROUGH){ // THROUGH, by default
            out_data_0 = in_data_0;
            out_data_1 = in_data_1;
        }
    }

    else if(residue_control==TO_RESIDUE){
        if(control == THROUGH){ // THROUGH, by default
            r_data_0 = in_data_0;
            r_data_1 = in_data_1;
        }
    }

   else if(residue_control==WITH_RESIDUE){
        if (control == SWITCH){
            out_data_0 = r_max_data_1;
            out_data_1 = r_max_data_0;
            r_data_0 = r_max_data_1;
            r_data_1 = r_max_data_0;
        }
        else if (control == MAX0){ 
            out_data_0 = r_max_data_0;
            out_data_1 = in_data_1;
            r_data_0 = r_max_data_0;
            r_data_1 = in_data_1;
        }
        else if (control == MAX1){
            out_data_0 = out_data_0;
            out_data_1 = r_max_data_1;
            r_data_0 = out_data_0;
            r_data_1 = r_max_data_1;
        }
        else if (control == MAX_THROUGH){
            out_data_0 = r_max_data_both;
            out_data_1 = r_max_data_both;
            r_data_0   = r_max_data_both;
            r_data_1   = r_max_data_both;
        }
        else if(control == THROUGH){ // THROUGH, by default
            out_data_0 = r_max_data_0;
            out_data_1 = r_max_data_1;
            r_data_0   = r_max_data_0;
            r_data_1   = r_max_data_1;
        }
    }
};



void afft_cmp16(
    hls::stream<ap_uint<AFFT_DATAWIDTH> >   data_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<OACTS_DATAWIDTH> >  output_stream[NUM_INPUT_DATA],
    hls::stream<ap_uint<NUM_CTRL_BITS>  >   control_stream[NUM_STAGE],
    int                                     afft_run_cycles
){
    ap_uint<AFFT_DATAWIDTH>   afft_inner_data[NUM_STAGE][NUM_INPUT_DATA];
    ap_uint<AFFT_DATAWIDTH>   rafft_res_data_0[NUM_INPUT_DATA/2];
    ap_uint<AFFT_DATAWIDTH>   rafft_res_data_1[NUM_INPUT_DATA/2];
    ap_uint<AFFT_DATAWIDTH>   rafft_res_data_both[NUM_INPUT_DATA/2];

    ap_uint<NUM_CTRL_BITS>    ctrl_reg[NUM_STAGE];
#pragma HLS ARRAY_PARTITION variable=afft_inner_data        dim=0 complete
#pragma HLS ARRAY_PARTITION variable=rafft_res_data_0       dim=0 complete
#pragma HLS ARRAY_PARTITION variable=rafft_res_data_1       dim=0 complete
#pragma HLS ARRAY_PARTITION variable=rafft_res_data_both    dim=0 complete
#pragma HLS array_partition variable=ctrl_reg               dim=0 complete 

    AFFT_COMPLETE_FLOW:
    for(int cycle=0; cycle < afft_run_cycles; cycle++){

        CONTROL_STREAM_FLOW:
        for(int i=0; i < NUM_STAGE; i++){
        #pragma HLS UNROLL
            ctrl_reg[i] = control_stream[i].read();
        }

        int read_counter = 0;
        INPUT_STREAM_FLOW:
        for(int j=0; j < NUM_INPUT_DATA; j++){
        #pragma HLS UNROLL
            afft_inner_data[0][j] = data_stream[j].read();
            read_counter++;
        } 

    #pragma HLS DATAFLOW
        // stage 1
        afft_switch(afft_inner_data[0][ 0], afft_inner_data[0][ 1], afft_inner_data[1][ 0], afft_inner_data[1][ 2], ctrl_reg[0].range( 2, 0));
        afft_switch(afft_inner_data[0][ 2], afft_inner_data[0][ 3], afft_inner_data[1][ 1], afft_inner_data[1][ 3], ctrl_reg[0].range( 5, 3));
        afft_switch(afft_inner_data[0][ 4], afft_inner_data[0][ 5], afft_inner_data[1][ 4], afft_inner_data[1][ 6], ctrl_reg[0].range( 8, 6));
        afft_switch(afft_inner_data[0][ 6], afft_inner_data[0][ 7], afft_inner_data[1][ 5], afft_inner_data[1][ 7], ctrl_reg[0].range(11, 9));
        afft_switch(afft_inner_data[0][ 8], afft_inner_data[0][ 9], afft_inner_data[1][ 8], afft_inner_data[1][10], ctrl_reg[0].range(14,12));
        afft_switch(afft_inner_data[0][10], afft_inner_data[0][11], afft_inner_data[1][ 9], afft_inner_data[1][11], ctrl_reg[0].range(17,15));
        afft_switch(afft_inner_data[0][12], afft_inner_data[0][13], afft_inner_data[1][12], afft_inner_data[1][14], ctrl_reg[0].range(20,18));
        afft_switch(afft_inner_data[0][14], afft_inner_data[0][15], afft_inner_data[1][13], afft_inner_data[1][15], ctrl_reg[0].range(23,21));
        
        // stage 2
        afft_switch(afft_inner_data[1][ 0], afft_inner_data[1][ 1], afft_inner_data[2][ 0], afft_inner_data[2][ 2], ctrl_reg[1].range( 2, 0));
        afft_switch(afft_inner_data[1][ 2], afft_inner_data[1][ 3], afft_inner_data[2][ 4], afft_inner_data[2][ 6], ctrl_reg[1].range( 5, 3));
        afft_switch(afft_inner_data[1][ 4], afft_inner_data[1][ 5], afft_inner_data[2][ 1], afft_inner_data[2][ 3], ctrl_reg[1].range( 8, 6));
        afft_switch(afft_inner_data[1][ 6], afft_inner_data[1][ 7], afft_inner_data[2][ 5], afft_inner_data[2][ 7], ctrl_reg[1].range(11, 9));
        afft_switch(afft_inner_data[1][ 8], afft_inner_data[1][ 9], afft_inner_data[2][ 8], afft_inner_data[2][10], ctrl_reg[1].range(14,12));
        afft_switch(afft_inner_data[1][10], afft_inner_data[1][11], afft_inner_data[2][12], afft_inner_data[2][14], ctrl_reg[1].range(17,15));
        afft_switch(afft_inner_data[1][12], afft_inner_data[1][13], afft_inner_data[2][ 9], afft_inner_data[2][11], ctrl_reg[1].range(20,18));
        afft_switch(afft_inner_data[1][14], afft_inner_data[1][15], afft_inner_data[2][13], afft_inner_data[2][15], ctrl_reg[1].range(23,21));
        
        // stage 3
        afft_switch(afft_inner_data[2][ 0], afft_inner_data[2][ 1], afft_inner_data[3][ 0], afft_inner_data[3][ 2], ctrl_reg[2].range( 2, 0));
        afft_switch(afft_inner_data[2][ 2], afft_inner_data[2][ 3], afft_inner_data[3][ 4], afft_inner_data[3][ 6], ctrl_reg[2].range( 5, 3));
        afft_switch(afft_inner_data[2][ 4], afft_inner_data[2][ 5], afft_inner_data[3][ 8], afft_inner_data[3][10], ctrl_reg[2].range( 8, 6));
        afft_switch(afft_inner_data[2][ 6], afft_inner_data[2][ 7], afft_inner_data[3][12], afft_inner_data[3][14], ctrl_reg[2].range(11, 9));
        afft_switch(afft_inner_data[2][ 8], afft_inner_data[2][ 9], afft_inner_data[3][ 1], afft_inner_data[3][ 3], ctrl_reg[2].range(14,12));
        afft_switch(afft_inner_data[2][10], afft_inner_data[2][11], afft_inner_data[3][ 5], afft_inner_data[3][ 7], ctrl_reg[2].range(17,15));
        afft_switch(afft_inner_data[2][12], afft_inner_data[2][13], afft_inner_data[3][ 9], afft_inner_data[3][11], ctrl_reg[2].range(20,18));
        afft_switch(afft_inner_data[2][14], afft_inner_data[2][15], afft_inner_data[3][13], afft_inner_data[3][15], ctrl_reg[2].range(23,21));
        
        // stage 4
        afft_switch(afft_inner_data[3][ 0], afft_inner_data[3][ 1], afft_inner_data[4][ 0], afft_inner_data[4][ 8], ctrl_reg[3].range( 2, 0));
        afft_switch(afft_inner_data[3][ 2], afft_inner_data[3][ 3], afft_inner_data[4][ 1], afft_inner_data[4][ 9], ctrl_reg[3].range( 5, 3));
        afft_switch(afft_inner_data[3][ 4], afft_inner_data[3][ 5], afft_inner_data[4][ 2], afft_inner_data[4][10], ctrl_reg[3].range( 8, 6));
        afft_switch(afft_inner_data[3][ 6], afft_inner_data[3][ 7], afft_inner_data[4][ 3], afft_inner_data[4][11], ctrl_reg[3].range(11, 9));
        afft_switch(afft_inner_data[3][ 8], afft_inner_data[3][ 9], afft_inner_data[4][ 4], afft_inner_data[4][12], ctrl_reg[3].range(14,12));
        afft_switch(afft_inner_data[3][10], afft_inner_data[3][11], afft_inner_data[4][ 5], afft_inner_data[4][13], ctrl_reg[3].range(17,15));
        afft_switch(afft_inner_data[3][12], afft_inner_data[3][13], afft_inner_data[4][ 6], afft_inner_data[4][14], ctrl_reg[3].range(20,18));
        afft_switch(afft_inner_data[3][14], afft_inner_data[3][15], afft_inner_data[4][ 7], afft_inner_data[4][15], ctrl_reg[3].range(23,21));
        
        // stage 5
        afft_switch(afft_inner_data[4][ 0], afft_inner_data[4][ 1], afft_inner_data[5][ 0], afft_inner_data[5][ 4], ctrl_reg[4].range( 2, 0));
        afft_switch(afft_inner_data[4][ 2], afft_inner_data[4][ 3], afft_inner_data[5][ 1], afft_inner_data[5][ 5], ctrl_reg[4].range( 5, 3));
        afft_switch(afft_inner_data[4][ 4], afft_inner_data[4][ 5], afft_inner_data[5][ 2], afft_inner_data[5][ 6], ctrl_reg[4].range( 8, 6));
        afft_switch(afft_inner_data[4][ 6], afft_inner_data[4][ 7], afft_inner_data[5][ 3], afft_inner_data[5][ 7], ctrl_reg[4].range(11, 9));
        afft_switch(afft_inner_data[4][ 8], afft_inner_data[4][ 9], afft_inner_data[5][ 8], afft_inner_data[5][12], ctrl_reg[4].range(14,12));
        afft_switch(afft_inner_data[4][10], afft_inner_data[4][11], afft_inner_data[5][ 9], afft_inner_data[5][13], ctrl_reg[4].range(17,15));
        afft_switch(afft_inner_data[4][12], afft_inner_data[4][13], afft_inner_data[5][10], afft_inner_data[5][14], ctrl_reg[4].range(20,18));
        afft_switch(afft_inner_data[4][14], afft_inner_data[4][15], afft_inner_data[5][11], afft_inner_data[5][15], ctrl_reg[4].range(23,21));

        // stage 6
        rafft_switch(afft_inner_data[5][ 0], afft_inner_data[5][ 1], afft_inner_data[6][ 0], afft_inner_data[6][ 2], rafft_res_data_0[0], rafft_res_data_1[0], rafft_res_data_both[0], ctrl_reg[5].range( 2, 0), ctrl_reg[6].range( 2, 0) );
        rafft_switch(afft_inner_data[5][ 2], afft_inner_data[5][ 3], afft_inner_data[6][ 1], afft_inner_data[6][ 3], rafft_res_data_0[1], rafft_res_data_1[1], rafft_res_data_both[1], ctrl_reg[5].range( 5, 3), ctrl_reg[6].range( 5, 3) );
        rafft_switch(afft_inner_data[5][ 4], afft_inner_data[5][ 5], afft_inner_data[6][ 4], afft_inner_data[6][ 6], rafft_res_data_0[2], rafft_res_data_1[2], rafft_res_data_both[2], ctrl_reg[5].range( 8, 6), ctrl_reg[6].range( 8, 6) );
        rafft_switch(afft_inner_data[5][ 6], afft_inner_data[5][ 7], afft_inner_data[6][ 5], afft_inner_data[6][ 7], rafft_res_data_0[3], rafft_res_data_1[3], rafft_res_data_both[3], ctrl_reg[5].range(11, 9), ctrl_reg[6].range(11, 9) );
        rafft_switch(afft_inner_data[5][ 8], afft_inner_data[5][ 9], afft_inner_data[6][ 8], afft_inner_data[6][10], rafft_res_data_0[4], rafft_res_data_1[4], rafft_res_data_both[4], ctrl_reg[5].range(14,12), ctrl_reg[6].range(14,12) );
        rafft_switch(afft_inner_data[5][10], afft_inner_data[5][11], afft_inner_data[6][ 9], afft_inner_data[6][11], rafft_res_data_0[5], rafft_res_data_1[5], rafft_res_data_both[5], ctrl_reg[5].range(17,15), ctrl_reg[6].range(17,15) );
        rafft_switch(afft_inner_data[5][12], afft_inner_data[5][13], afft_inner_data[6][12], afft_inner_data[6][14], rafft_res_data_0[6], rafft_res_data_1[6], rafft_res_data_both[6], ctrl_reg[5].range(20,18), ctrl_reg[6].range(20,18) );
        rafft_switch(afft_inner_data[5][14], afft_inner_data[5][15], afft_inner_data[6][13], afft_inner_data[6][15], rafft_res_data_0[7], rafft_res_data_1[7], rafft_res_data_both[7], ctrl_reg[5].range(23,21), ctrl_reg[6].range(23,21) );


        //&& uncomment to debug ---- comment to synthesize
/*

        std::cout << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
        std::cout << "Cycle =" << cycle << std::endl;
        // printing content of afft
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range( 2, 0)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range( 2, 0)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range( 2, 0)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range( 2, 0)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range( 2, 0)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range( 2, 0)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range( 2, 0)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range( 5, 3)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range( 5, 3)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range( 5, 3)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range( 5, 3)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range( 5, 3)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range( 5, 3)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range( 5, 3)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range( 8, 6)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range( 8, 6)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range( 8, 6)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range( 8, 6)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range( 8, 6)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range( 8, 6)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range( 8, 6)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range(11, 9)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range(11, 9)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range(11, 9)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range(11, 9)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range(11, 9)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range(11, 9)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range(11, 9)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range(14,12)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range(14,12)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range(14,12)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range(14,12)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range(14,12)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range(14,12)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range(14,12)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range(17,15)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range(17,15)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range(17,15)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range(17,15)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range(17,15)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range(17,15)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range(17,15)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range(20,18)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range(20,18)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range(20,18)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range(20,18)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range(20,18)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range(20,18)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range(20,18)) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[0].range(23,21)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[1].range(23,21)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[2].range(23,21)) << " " << std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[3].range(23,21)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[4].range(23,21)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[5].range(23,21)) << " "<< std::dec << std::setw(8) << static_cast<uint>(ctrl_reg[6].range(23,21)) << " " << std::endl;

        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;

        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 0]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 0]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 0]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 0]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 0]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 0]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 0]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 1]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 1]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 1]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 1]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 1]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 1]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 1]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 2]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 2]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 2]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 2]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 2]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 2]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 2]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 3]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 3]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 3]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 3]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 3]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 3]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 3]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 4]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 4]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 4]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 4]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 4]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 4]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 4]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 5]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 5]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 5]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 5]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 5]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 5]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 5]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 6]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 6]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 6]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 6]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 6]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 6]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 6]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 7]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 7]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 7]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 7]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 7]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 7]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 7]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 8]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 8]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 8]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 8]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 8]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 8]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 8]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][ 9]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][ 9]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][ 9]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][ 9]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][ 9]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][ 9]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][ 9]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][10]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][10]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][10]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][10]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][10]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][10]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][10]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][11]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][11]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][11]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][11]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][11]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][11]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][11]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][12]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][12]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][12]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][12]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][12]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][12]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][12]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][13]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][13]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][13]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][13]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][13]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][13]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][13]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][14]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][14]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][14]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][14]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][14]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][14]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][14]) << " " << std::endl;
        std::cout << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[0][15]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[1][15]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[2][15]) << " " << std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[3][15]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[4][15]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[5][15]) << " "<< std::dec << std::setw(8) << static_cast<uint>(afft_inner_data[6][15]) << " " << std::endl;
        std::cout << "///////////////////////////////////////////////////////////////////////////////" << std::endl;
        
*/
        OUTPUT_STREAM_FLOW:
        for(int j=0; j < NUM_INPUT_DATA; j++){
        #pragma HLS UNROLL
            ap_uint<OACTS_DATAWIDTH> temp_write = afft_inner_data[6][j];
            output_stream[j].write(temp_write);
        }
    }
}


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
){
    //Take 16 data at a time (1 Row) - act on 9 
    // Access patterns - (9,6)
    int  stage_4_seq_cntr_internal          = 0;
    int  output_stage_cntr_internal         = 0;
    int  read_data_from_stat_buff_internal  = 0;
    bool enable_stage_4_seq_cntr_internal   = false;
    bool enable_stage_5_seq_cntr_internal   = false;
    bool enable_output_stage_cntr_internal  = false;
    bool afft_routing_reset_internal        = false;
    int  afft_routing_limit_internal        = std::min(maxpool_w_out-1, 15); // must reset the routing when either one row of the oacts is filled (maxpool_w_out reached), or afft limit is reached
    int write_source=0; 
    
    ap_uint<NUM_CTRL_BITS>                  ctrl_word[NUM_STAGE];

    for(int f=0; f<read_addr_from_buff_end; f++){
        // Reading data from buffer
        ap_uint<HP_IFC_BANDWIDTH> iacts_data_to_put_in_stream = buff_iacts[read_addr_from_buff_start + f];

        // control logic generation
        if(read_data_from_stat_buff_internal != afft_routing_limit){
            read_data_from_stat_buff_internal = (read_data_from_stat_buff_internal + 1);
            if(read_data_from_stat_buff_internal == 5){
                enable_stage_4_seq_cntr_internal = true;
            }
            if(read_data_from_stat_buff_internal == 7){
                enable_output_stage_cntr_internal = true;
            }
        }
        else{
            read_data_from_stat_buff_internal = 0;
        }

        if(enable_stage_4_seq_cntr_internal == true){
            if(stage_4_seq_cntr_internal != afft_routing_limit){
                stage_4_seq_cntr_internal = (stage_4_seq_cntr_internal + 1);
            }
            else{
                stage_4_seq_cntr_internal = 0;
            }
        }
        if(enable_output_stage_cntr_internal == true){
            if(output_stage_cntr_internal != afft_routing_limit){
                output_stage_cntr_internal = (output_stage_cntr_internal + 1);
            }
            else{
                output_stage_cntr_internal = 0;
            }
        }

        ap_uint<HP_IFC_BANDWIDTH> ctrl_data_to_put_in_stream = buff_rafft_ctrl_reg[read_addr_from_buff_start + f];

        
        if(f==0){
            ctrl_word[0] = ctrl_data_to_put_in_stream.range( 23 , 0);
            ctrl_word[1] = ctrl_data_to_put_in_stream.range( 47, 24);
            ctrl_word[2] = ctrl_data_to_put_in_stream.range( 71, 48);
            ctrl_word[3] = ctrl_data_to_put_in_stream.range( 95, 72);
            ctrl_word[6] = ctrl_data_to_put_in_stream.range(120, 96);
        }
        else{
            ctrl_word[4] = ctrl_data_to_put_in_stream.range( 23 , 0);
            ctrl_word[6] = ctrl_data_to_put_in_stream.range( 71, 48);
            ctrl_word[5] = ctrl_data_to_put_in_stream.range( 47, 24);
        }


        stage_4_seq_cntr    = stage_4_seq_cntr_internal;
        enable_output_stage_cntr = enable_output_stage_cntr_internal;
        afft_routing_reset  = afft_routing_reset_internal;
        afft_routing_limit  = afft_routing_limit_internal;

        // writing the read data to the stream
        for(int ff=0; ff<WORDS_PER_ADDR; ff++){
        #pragma HLS UNROLL
            ap_uint<IACTS_DATAWIDTH> iacts_data_to_put_in_stream_split = iacts_data_to_put_in_stream.range((ff*8) + 7, (ff*8));
            afft_input_data_stream[ff].write(iacts_data_to_put_in_stream_split);
        }

        // writing the control to the stream
        for(int cc=0; cc<7; cc++){
        #pragma HLS UNROLL
            ap_uint<NUM_CTRL_BITS> ctrl_temp = ctrl_word[cc];
            afft_input_ctrl_stream[cc].write(ctrl_temp);
            write_source++;
        }
    }
}

void get_data_from_afft(
    hls::stream<ap_uint<OACTS_DATAWIDTH> >  afft_output_data_stream[NUM_INPUT_DATA],
    ap_uint<HP_IFC_BANDWIDTH>               buff_in2[RW_SIZE],
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
){
    auto  buff_shift =0;
    ap_uint<HP_IFC_BANDWIDTH> data_to_store_in_buff=0;
    ap_uint<HP_IFC_BANDWIDTH> data_to_store_in_buff_2=0;
    for(int dd=0; dd<write_addr_from_buff_end; dd++){
        for(int ff=0; ff<WORDS_PER_ADDR; ff++){
        #pragma HLS UNROLL
            ap_uint<OACTS_DATAWIDTH> data_from_afft_stream  = afft_output_data_stream[ff].read();
            data_to_store_in_buff.range((ff*OACTS_DATAWIDTH) + OACTS_DATAWIDTH-1, (ff*OACTS_DATAWIDTH)) = data_from_afft_stream;
        }
        buff_shift++;
        data_to_store_in_buff_2 = data_to_store_in_buff_2 >> 16;
        data_to_store_in_buff_2.range(127,112) = data_to_store_in_buff.range(15, 0);
    }
        buff_in2[write_addr_from_buff_start] = data_to_store_in_buff_2;
}

#endif

void maeri_v2_1(
    ap_uint<HP_IFC_BANDWIDTH> *ifc1,
    ap_uint<HP_IFC_BANDWIDTH> *ifc2,
    ap_uint<HP_IFC_BANDWIDTH> *ifc3,
    ap_uint<HP_IFC_BANDWIDTH> *ifc4,
    ap_uint<HP_IFC_BANDWIDTH> *ifc5,
    ap_uint<HP_IFC_BANDWIDTH> *ifc6,
    int iacts_zp,
    int num_ifc_entry_iacts

)
{
#pragma HLS INTERFACE m_axi offset = slave port = ifc1 bundle = port1 depth = RW_SIZE max_read_burst_length = 128 max_write_burst_length = 128
#pragma HLS INTERFACE m_axi offset = slave port = ifc2 bundle = port2 depth = RW_SIZE max_read_burst_length = 128 max_write_burst_length = 128
#pragma HLS INTERFACE m_axi offset = slave port = ifc3 bundle = port3 depth = RW_SIZE max_read_burst_length = 128 max_write_burst_length = 128
#pragma HLS INTERFACE m_axi offset = slave port = ifc4 bundle = port4 depth = RW_SIZE max_read_burst_length = 128 max_write_burst_length = 128
#pragma HLS INTERFACE m_axi offset = slave port = ifc5 bundle = port5 depth = RW_SIZE max_read_burst_length = 128 max_write_burst_length = 128
#pragma HLS INTERFACE m_axi offset = slave port = ifc6 bundle = port6 depth = RW_SIZE max_read_burst_length = 128 max_write_burst_length = 128
#pragma HLS INTERFACE s_axilite register port = return

    int i;
    ap_uint<HP_IFC_BANDWIDTH> stationary_buffer[RW_SIZE];
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> buff_in2[RW_SIZE];
    ap_uint<HP_IFC_BANDWIDTH> buff_in3[RW_SIZE];
    ap_uint<HP_IFC_BANDWIDTH> buff_in4[RW_SIZE];
    ap_uint<HP_IFC_BANDWIDTH> buff_out[RW_SIZE];

#pragma HLS ARRAY_PARTITION variable = stationary_buffer type = cyclic factor = 4
#pragma HLS ARRAY_PARTITION variable = buff_in2 type = cyclic factor = 4
#pragma HLS BIND_STORAGE variable = stationary_buffer type = ram_t2p impl = bram

    if (num_ifc_entry_iacts < 257){
GET_DATA:
        for (int i = 0; i < num_ifc_entry_iacts; i++){
#pragma HLS loop_tripcount min=257 max=257
#pragma HLS DEPENDENCE variable = stationary_buffer intra false
        STORE_BUFF_1_0:
            stationary_buffer[(0 * (STEP_STATIONARY_BUFF)) + i] = ifc1[i];
        STORE_BUFF_2_0:
            stationary_buffer[(1 * (STEP_STATIONARY_BUFF)) + i] = ifc2[i];
        STORE_BUFF_3_0:
            stationary_buffer[(2 * (STEP_STATIONARY_BUFF)) + i] = ifc3[i];
        STORE_BUFF_4_0:
            stationary_buffer[(3 * (STEP_STATIONARY_BUFF)) + i] = ifc4[i];
        STORE_BUFF_5_0:
            stationary_buffer[(4 * (STEP_STATIONARY_BUFF)) + i] = ifc5[i];
        STORE_BUFF_6_0:
            stationary_buffer[(5 * (STEP_STATIONARY_BUFF)) + i] = ifc6[i];
        }
    }

    ap_uint <IACTS_DATAWIDTH> zp = iacts_zp;


#ifdef RELU_ENABLED
//-----------------------------------------------------------------------------------------------//
//----------------------------------- ReLU Activation begin -------------------------------------//
//-----------------------------------------------------------------------------------------------//

// Shift register based implementation to resolve WAR dependency on a location
// read from addr 0, store in shift_reg[0]
// shift the reg to the right by addr shift_reg[0] -> shift_reg[1]
// read from addr 1, store in shift_reg[0], simultaenously perform relu on data in shift_reg[1]
// write content of shift_reg[1] in addr 0, at same time read from shift_reg[0].... repeat

    ap_uint<HP_IFC_BANDWIDTH> relu_shift_reg_arr[NUM_ALIGNED_BUFFER][2]={0};
    ap_uint<HP_IFC_BANDWIDTH> relued_output[NUM_ALIGNED_BUFFER]={0};
#pragma HLS array_partition variable=relu_shift_reg_arr type=complete
#pragma HLS array_partition variable=relued_output type=complete

    RELU:
    for(int i=0; i<num_ifc_entry_iacts+1; i=i+1){
#pragma HLS loop_tripcount min=257 max=257
#pragma HLS DEPENDENCE variable=stationary_buffer intra false 
#pragma HLS PIPELINE II=1
        for(int ll=0; ll<NUM_ALIGNED_BUFFER; ll++){
#pragma HLS UNROLL
        // to not read any extra data
            if(i<num_ifc_entry_iacts){
                relu_shift_reg_arr[ll][0] = stationary_buffer[i + (ll*(STEP_STATIONARY_BUFF))];
            }
            if(i > 0){
                relued_output[ll]    = relu_16words_8bit(relu_shift_reg_arr[ll][1], zp);
                stationary_buffer[i - 1 + (ll*(STEP_STATIONARY_BUFF))]   = relued_output[ll];
            }
            relu_shift_reg_arr[ll][1] = relu_shift_reg_arr[ll][0];
        }
    }

//-----------------------------------------------------------------------------------------------//
//----------------------------------- ReLU Activation end ---------------------------------------//
//-----------------------------------------------------------------------------------------------//
#endif

#ifdef BN_ENABLED
//-----------------------------------------------------------------------------------------------//
//-----------------------------------BN Function call begin--------------------------------------//
//-----------------------------------------------------------------------------------------------//
    int ti_iter = 0;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> bn_m_factor;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> bn_c_factor;
    ap_fixed<BN_COEFF_FIX_POINT_BITWIDTH, BN_COEFF_INT_BITWIDTH, AP_RND_CONV, AP_SAT> inv_scale;
    
    ap_uint<UB_WIDTH>    bn_buffer[MAX_STREAMING_BUF_ENTRY_HALF];
    #pragma HLS BIND_STORAGE variable=bn_buffer type=ram_t2p impl=bram latency=1
    #pragma HLS array_partition variable=bn_buffer type=cyclic factor=4 dim=0

    int init_ifc_addr_read = 0; //&&

    BNCall (    num_ifc_entry_iacts,
                stationary_buffer,
                ifc1,
                ifc2,
                ifc3, 
                ifc4,
                ifc5,
                ifc6, 
                bn_buffer, 
                init_ifc_addr_read,
                bn_m_factor, 
                bn_c_factor, 
                inv_scale, 
                zp
            );

//-----------------------------------------------------------------------------------------------//
//-----------------------------------BN Function call end----------------------------------------//
//-----------------------------------------------------------------------------------------------//
#endif


#ifdef MAXPOOL_ENABLED
//-----------------------------------------------------------------------------------------------//
//----------------------------------- MaxPool     begin -----------------------------------------//
//-----------------------------------------------------------------------------------------------//
    hls::stream<ap_uint<IACTS_DATAWIDTH> >  afft_input_data_stream[NUM_INPUT_DATA];
    hls::stream<ap_uint<NUM_CTRL_BITS> >    afft_input_ctrl_stream[NUM_STAGE];
    hls::stream<ap_uint<OACTS_DATAWIDTH> >  afft_output_data_stream[NUM_INPUT_DATA];

    int                                     init_burst_array_row_id_iacts=0;
    int                                     stage_4_seq_cntr=0;
    int                                     afft_routing_limit=0;
    bool                                    afft_routing_reset=0;
    bool                                    enable_output_stage_cntr=0;
    int                                     read_addr_from_buff_start;
    int                                     read_addr_from_buff_end;
    int                                     num_ifc_entry_iacts2;
    int                                     kernel_size;
    int                                     maxpool_w_out;
    int                                     maxpool_h_out;
    int                                     num_oacts;
    int                                     write_addr_from_buff_start;
    int                                     write_addr_from_buff_end;


    ap_uint<UB_WIDTH>                                                  rafft_ctrl_buffer[MAX_UB_BUF_ENTRY];
    #pragma HLS BIND_STORAGE variable=rafft_ctrl_buffer type=ram_t2p impl=uram latency=1
    #pragma HLS array_partition variable=rafft_ctrl_buffer type=cyclic factor=4 dim=0

    FILL_MAXPOOL_CTRL_BUFF:
    for(int i=0; i<num_ifc_entry_iacts; i++){
#pragma HLS loop_tripcount min=257 max=257 avg=257
#pragma HLS DEPENDENCE variable=rafft_ctrl_buffer intra false
        rafft_ctrl_buffer[(0*(STEP_STATIONARY_BUFF)) + i] = ifc5[i + init_burst_array_row_id_iacts];
        rafft_ctrl_buffer[(1*(STEP_STATIONARY_BUFF)) + i] = ifc6[i + init_burst_array_row_id_iacts];
    }

    std::cout << "+++++++++++++++++++++++++++++++++++++++++++++" << std::endl;

    read_addr_from_buff_start   = rafft_ctrl_buffer[0].range(2,0);
    read_addr_from_buff_end     = read_addr_from_buff_start + num_ifc_entry_iacts;
    num_ifc_entry_iacts2        = num_ifc_entry_iacts;
    if(rafft_ctrl_buffer[3].range(1,0) == 0){
        kernel_size             = 3;
    }
    else if(rafft_ctrl_buffer[3].range(1,0) == 1){
        kernel_size             = 3;
    }
    else if(rafft_ctrl_buffer[3].range(1,0) == 2){
        kernel_size             = 5;
    }
    else if(rafft_ctrl_buffer[3].range(1,0) == 3){
        kernel_size             = 7;
    }
    maxpool_w_out               = rafft_ctrl_buffer[4].range(3,0);
    maxpool_h_out               = maxpool_w_out;
    num_oacts                   = rafft_ctrl_buffer[6].range(3,0);
    write_addr_from_buff_start  = rafft_ctrl_buffer[0].range(2,0);
    write_addr_from_buff_end    = write_addr_from_buff_start + num_ifc_entry_iacts;
//    read_addr_from_buff_start   = rafft_ctrl_buffer[0].range(2,0);
//    read_addr_from_buff_end     = rafft_ctrl_buffer[1].range(3,0);
//    num_ifc_entry_iacts2        = rafft_ctrl_buffer[2].range(3,0);
//    kernel_size                 = rafft_ctrl_buffer[3].range(2,0);
//    maxpool_w_out               = rafft_ctrl_buffer[4].range(3,0);
//    maxpool_h_out               = rafft_ctrl_buffer[5].range(3,0);
//    num_oacts                   = rafft_ctrl_buffer[6].range(3,0);
//    write_addr_from_buff_start  = rafft_ctrl_buffer[0].range(2,0);
//    write_addr_from_buff_end    = rafft_ctrl_buffer[1].range(3,0);

    feed_data_to_afft(
        stationary_buffer,
        rafft_ctrl_buffer,
        afft_input_data_stream,
        afft_input_ctrl_stream,
        read_addr_from_buff_start,
        read_addr_from_buff_end,
        num_ifc_entry_iacts,
        kernel_size,
        maxpool_w_out,
        maxpool_h_out,
        num_oacts,
        stage_4_seq_cntr,
        enable_output_stage_cntr,
        afft_routing_reset,
        afft_routing_limit
        );

    afft_cmp16(
        afft_input_data_stream,
        afft_output_data_stream,
        afft_input_ctrl_stream,
        read_addr_from_buff_end
        );

    get_data_from_afft(
        afft_output_data_stream,
        stationary_buffer,
        write_addr_from_buff_start,
        write_addr_from_buff_end,
        num_ifc_entry_iacts,
        maxpool_w_out,
        maxpool_h_out,
        num_oacts,
        stage_4_seq_cntr,
        enable_output_stage_cntr,
        afft_routing_reset,
        afft_routing_limit
        );

//-----------------------------------------------------------------------------------------------//
//----------------------------------- MaxPool     End -------------------------------------------//
//-----------------------------------------------------------------------------------------------//
#endif



 // TEMPORARY LOGIC TO SEND SOME JUNK OUTPUT TO SEE SYNTHESIS REPORT
    for(int ll=0; ll<256; ll++){
        ifc1[ll] = stationary_buffer[ll];
    }


}