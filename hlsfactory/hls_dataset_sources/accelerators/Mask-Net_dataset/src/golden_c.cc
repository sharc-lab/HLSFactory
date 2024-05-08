
#include "net_hls.h"
#include "hls_stream.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <ap_fixed.h>

using namespace std;
#define RELEASE
//#define _CRT_SECURE_NO_WARNINGS

 FILE* fo;

/////// Golden Model Variables
extern float image[3][160][320];
extern float dw1_conv_3x3_weight[3][3][3];
extern float dw1_conv_3x3_bias[3];
extern float dw1_conv_3x3_output[3][160][320];

extern float dw1_conv_1x1_weight[48][3];
extern float dw1_conv_1x1_bias[48];
extern float dw1_conv_1x1_output[48][160][320];

extern float dw1_max_pool_output[48][80][160];

extern float dw2_conv_3x3_weight[48][3][3];
extern float dw2_conv_3x3_bias[48];
extern float dw2_conv_3x3_output[48][80][160];

extern float dw2_conv_1x1_weight[96][48];
extern float dw2_conv_1x1_bias[96];
extern float dw2_conv_1x1_output[96][80][160];

extern float dw2_max_pool_output[96][40][80];

extern float dw3_conv_3x3_weight[96][3][3];
extern float dw3_conv_3x3_bias[96];
extern float dw3_conv_3x3_output[96][40][80];

extern float dw3_conv_1x1_weight[192][96];
extern float dw3_conv_1x1_bias[192];
extern float dw3_conv_1x1_output[192][40][80];

extern float dw3_max_pool_output[192][20][40];

extern float dw4_conv_3x3_weight[192][3][3];
extern float dw4_conv_3x3_bias[192];
extern float dw4_conv_3x3_output[192][20][40];

extern float dw4_conv_1x1_weight[384][192];
extern float dw4_conv_1x1_bias[384];
extern float dw4_conv_1x1_output[384][20][40];

extern float dw5_conv_3x3_weight[384][3][3];
extern float dw5_conv_3x3_bias[384];
extern float dw5_conv_3x3_output[384][20][40];

extern float dw5_conv_1x1_weight[512][384];
extern float dw5_conv_1x1_bias[512];
extern float dw5_conv_1x1_output[512][20][40];

// cat dw3(ch:192 -> 768) and dw5(ch:512)
extern float dw6_conv_3x3_input[1280][20][40];
extern float dw6_conv_3x3_weight[1280][3][3];
extern float dw6_conv_3x3_bias[1280];
extern float dw6_conv_3x3_output[1280][20][40];

extern float dw6_conv_1x1_weight[96][1280];
extern float dw6_conv_1x1_bias[96];
extern float dw6_conv_1x1_output[96][20][40];

extern float pw7_conv_1x1_weight[10][96];
extern float pw7_conv_1x1_output[10][20][40];

// reordered weights for the mysterious dw3(192->768)
extern float dw6_conv_3x3_weight_reo[1280][3][3];
extern float dw6_conv_3x3_bias_reo[1280];
extern float dw6_conv_3x3_input_reo[1280][20][40];
extern float dw6_conv_3x3_output_reo[1280][20][40];

extern float dw6_conv_1x1_weight_reo[96][1280];
extern float dw6_conv_1x1_bias_reo[96];
extern float dw6_conv_1x1_output_reo[96][20][40];

//define the mask

//extern float dw3_mask[192][40][80];
extern float dw4__conv_3x3_mask[192][20][40];
extern float dw4__conv_1x1_mask[384][20][40];
extern float dw5__conv_3x3_mask[384][20][40];
extern float dw5__conv_1x1_mask[512][20][40];
extern float dw6_conv_3x3_mask[1280][20][40];
extern float dw6_conv_1x1_mask[96][20][40];

//define outputs behind the mask

extern float dw3_mask[192][40][80];
extern float dw4__conv_3x3_mask[192][20][40];
extern float dw4__conv_1x1_mask[384][20][40];
extern float dw5__conv_3x3_mask[384][20][40];
extern float dw5__conv_1x1_mask[512][20][40];
extern float dw6_conv_3x3_mask[1280][20][40];
extern float dw6_conv_1x1_mask[96][20][40];

//extern float new_branch_conv_5x5_max_pool_output[192][40][40];
extern float new_branch_conv_5x5_weight[32][6][5][5];
extern float new_branch_conv_5x5_bias[32];
extern float new_branch_conv_5x5_output[32][20][40];
//extern float new_branch_conv_3x3_max_pool_output[32][4][8];
extern float new_branch_conv_3x3_max_pool_output1[32][10][20];
extern float new_branch_conv_3x3_max_pool_output2[32][5][10];
extern float new_branch_conv_3x3_max_pool_output3[32][5][5];
extern float new_branch_conv_3x3_weight[1][32][3][3];
extern float mask_temp[1][5][5];
extern float mask[1][5][5];
float dw3_max_pool_output_reorg[192][20][40];
//extern float new_branch_conv_3x3_bias[16];

int mask_boundary[4];
int my_index[1];
int xmin[1];
int xmax[1];
int ymin[1];
int ymax[1];




float max_4(float a1, float a2, float a3, float a4)
{
    float tmp1, tmp2;

    if(a1 > a2) tmp1 = a1; else tmp1 = a2;
    if(a3 > a4) tmp2 = a3; else tmp2 = a4;
    if(tmp1 > tmp2) return tmp1; else return tmp2;
}

float max_2(float a1, float a2)
{
    float tmp1;

    if (a1 > a2) tmp1 = a1; else tmp1 = a2;
    return tmp1;
}

float max_5(float a1, float a2, float a3, float a4, float a5)
{
    float tmp1, tmp2, tmp3;
    if (a1 > a2) tmp1 = a1; else tmp1 = a2;
    if (a3 > a4) tmp2 = a3; else tmp2 = a4;
    if (tmp1 > tmp2) tmp3 = tmp1; else tmp3 = tmp2;
    if (tmp3 > a5) return tmp3; else return a5;
}

void mask_threshold(float input[1][5][5], 
                    float output[1][5][5])
{
    
    
    for (int h = 0; h < 5; h++) {
        for (int w = 0; w < 5; w++) {
            
            if (input[0][h][w] >0.3) output[0][h][w] = 1.0f; else output[0][h][w] = 0.0f;
        }
    }

    fo = fopen("mask_threshold", "w");
    for (int h = 0; h < 5; h++) {
        for (int w = 0; w < 5; w++) {
            {
                fprintf(fo, "output_mask[%d][%d][%d] = %.3f\n", 1,h,w, output[0][h][w]);
            }
        }
    }
    fclose(fo);
}




void dw1_conv_3x3(
            float input[3][160][320],
            float weight[3][3][3],
            float bias[3],
            float output[3][160][320]
            )
{
    //cout << "dw1_conv_3x3..." << endl;

    for(int co = 0; co < 3; co++) {
        for(int h = 0; h < 160; h++) {
            for(int w = 0; w < 320; w++) {
                float sum = 0;

                for(int m = 0; m < 3; m++) {
                    for(int n = 0; n < 3; n++) {

                        sum += weight[co][m][n] *
                                (( h+m-1 >= 0 && w+n-1 >= 0 && h+m-1 < 160 && w+n-1 < 320) ? input[co][h+m-1][w+n-1] : 0);


                    }
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw1_conv_3x3_output", "w");
    for(int i = 0; i < 3; i++) {
        for(int j = 0; j < 160; j++) {
            for(int k = 0; k < 320; k ++) {
                fprintf(fo, "dw1_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif

}


void dw1_conv_1x1(
            float input[3][160][320],
            float weight[48][3],
            float bias[48],
            float output[48][160][320]
            )
{
    //cout << "dw1_conv_1x1..." << endl;

    for(int co = 0; co < 48; co++) {
        for(int h = 0; h < 160; h++) {
            for(int w = 0; w < 320; w++) {
                float sum = 0;

                for(int ci = 0; ci < 3; ci++ ) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw1_conv_1x1_output", "w");
    for(int i = 0; i < 48; i++) {
        for(int j = 0; j < 160; j++) {
            for(int k = 0; k < 320; k ++) {
                fprintf(fo, "dw1_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}


void dw1_max_pool(
                   float input[48][160][320],
                   float output[48][80][160]
                   )
{
    //cout << "dw1_max_pool..." << endl;

    for(int co = 0; co < 48; co++) {
        for(int h = 0; h < 80; h++) {
            for(int w = 0; w < 160; w++) {

                output[co][h][w] = max_4(
                                        input[co][h*2][w*2],
                                        input[co][h*2+1][w*2],
                                        input[co][h*2][w*2+1],
                                        input[co][h*2+1][w*2+1]
                                        );
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw1_max_pool_output", "w");
    for(int i = 0; i < 48; i++) {
        for(int j = 0; j < 80; j++) {
            for(int k = 0; k < 160; k ++) {
                fprintf(fo, "dw1_max_pool_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif

}


void dw2_conv_3x3(
            float input[48][80][160],
            float weight[48][3][3],
            float bias[48],
            float output[48][80][160]
            )
{
    //cout << "dw2_conv_3x3..." << endl;

    for(int co = 0; co < 48; co++) {
        for(int h = 0; h < 80; h++) {
            for(int w = 0; w < 160; w++) {
                float sum = 0;

                for(int m = 0; m < 3; m++) {
                    for(int n = 0; n < 3; n++) {
                        sum += weight[co][m][n] *
                                (( h+m-1 >= 0 && w+n-1 >= 0 && h+m-1 < 80 && w+n-1 < 160) ? input[co][h+m-1][w+n-1] : 0);
                    }
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw2_conv_3x3_output", "w");
    for(int i = 0; i < 48; i++) {
        for(int j = 0; j < 80; j++) {
            for(int k = 0; k < 160; k ++) {
                fprintf(fo, "dw2_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif

}


void dw2_conv_1x1(
            float input[48][80][160],
            float weight[96][48],
            float bias[96],
            float output[96][80][160]
            )
{
    //cout << "dw2_conv_1x1..." << endl;

    for(int co = 0; co < 96; co++) {
        for(int h = 0; h < 80; h++) {
            for(int w = 0; w < 160; w++) {
                float sum = 0;

                for(int ci = 0; ci < 48; ci++ ) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw2_conv_1x1_output", "w");
    for(int i = 0; i < 96; i++) {
        for(int j = 0; j < 80; j++) {
            for(int k = 0; k < 160; k ++) {
                fprintf(fo, "dw2_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif

}


void dw2_max_pool(
                float input[96][80][160],
                float output[96][40][80]
                )
{
    //cout << "dw2_max_pool..." << endl;

    for(int co = 0; co < 96; co++) {
        for(int h = 0; h < 40; h++) {
            for(int w = 0; w < 80; w++) {

                output[co][h][w] = max_4(
                                        input[co][h*2][w*2],
                                        input[co][h*2+1][w*2],
                                        input[co][h*2][w*2+1],
                                        input[co][h*2+1][w*2+1]
                                        );
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw2_max_pool_output", "w");
    for(int i = 0; i < 96; i++) {
        for(int j = 0; j < 40; j++) {
            for(int k = 0; k < 80; k ++) {
                fprintf(fo, "dw2_max_pool_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif

}


void dw3_conv_3x3(
            float input[96][40][80],
            float weight[96][3][3],
            float bias[96],
            float output[96][40][80]
            )
{
    //cout << "dw3_conv_3x3..." << endl;

    for(int co = 0; co < 96; co++) {
        for(int h = 0; h < 40; h++) {
            for(int w = 0; w < 80; w++) {
                float sum = 0;

                for(int m = 0; m < 3; m++) {
                    for(int n = 0; n < 3; n++) {

                        sum += weight[co][m][n] *
                                (( h+m-1 >= 0 && w+n-1 >= 0 && h+m-1 < 40 && w+n-1 < 80) ? input[co][h+m-1][w+n-1] : 0);
                    }
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw3_conv_3x3_output", "w");
    for(int i = 0; i < 96; i++) {
        for(int j = 0; j < 40; j++) {
            for(int k = 0; k < 80; k ++) {
                fprintf(fo, "dw3_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}


void dw3_conv_1x1(
            float input[96][40][80],
            float weight[192][96],
            float bias[192],
            float output[192][40][80]
            )
{
    //cout << "dw3_conv_1x1..." << endl;

    for(int co = 0; co < 192; co++) {
        for(int h = 0; h < 40; h++) {
            for(int w = 0; w < 80; w++) {
                float sum = 0;

                for(int ci = 0; ci < 96; ci++ ) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }

    /*fo = fopen("dw3_conv_1x1_output", "w");
    for(int i = 0; i < 192; i++) {
        for(int j = 0; j < 40; j++) {
            for(int k = 0; k < 80; k ++) {
                fprintf(fo, "dw3_conv_1x1_output[%d][%d][%d] = %.5f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);*/

}


//void dw3_max_pool(
//                float input[192][40][80],
//                float output[192][20][40],
//                float mask[192][40][80]
//                )
//{
//    //cout << "dw3_max_pool..." << endl;
//
//    for(int co = 0; co < 192; co++) {
//        for(int h = 0; h < 20; h++) {
//            for(int w = 0; w < 40; w++) {
//
//                output[co][h][w] = max_4(
//                                        input[co][h*2][w*2] * mask[co][h * 2][w * 2],
//                                        input[co][h*2+1][w*2] * mask[co][h * 2 + 1][w * 2],
//                                        input[co][h*2][w*2+1] * mask[co][h * 2][w * 2 + 1],
//                                        input[co][h*2+1][w*2+1] * mask[co][h * 2 + 1][w * 2 + 1]
//                                        );
//            }
//        }
//    }


void dw3_max_pool(
    float input[192][40][80],
    float output[192][20][40]
    //float mask[192][40][80]
)
{
    //cout << "dw3_max_pool..." << endl;

    for (int co = 0; co < 192; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {

                output[co][h][w] = max_4(
                    input[co][h * 2][w * 2] ,
                    input[co][h * 2 + 1][w * 2] ,
                    input[co][h * 2][w * 2 + 1],
                    input[co][h * 2 + 1][w * 2 + 1] 
                );
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw3_max_pool_output", "w");
    for(int i = 0; i < 192; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw3_max_pool_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}



void dw4_conv_3x3(
            float input[192][20][40],
            float weight[192][3][3],
            float bias[192],
            float output[192][20][40],
            //float mask[192][20][40]
            int mask_boundary[4]
            )
{
    //cout << "dw4_conv_3x3..." << endl;

    for(int co = 0; co < 192; co++) {
        for(int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for(int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for(int m = 0; m < 3; m++) {
                    for(int n = 0; n < 3; n++) {

                        sum += weight[co][m][n] *
                                (( h+m-1 >= mask_boundary[0] && w+n-1 >= mask_boundary[2] && h+m-1 < mask_boundary[1] && w+n-1 < mask_boundary[3]) ? input[co][h+m-1][w+n-1] : 0);
                    }
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw4_conv_3x3_output", "w");
    for(int i = 0; i < 192; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw4_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}


void dw4_conv_1x1(
            float input[192][20][40],
            float weight[384][192],
            float bias[384],
            float output[384][20][40],
            //float mask[192][20][40]
            int mask_boundary[4]
            )
{
    //cout << "dw4_conv_1x1..." << endl;

    for(int co = 0; co < 384; co++) {
        for(int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for(int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for(int ci = 0; ci < 192; ci++ ) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                float result = sum + bias[co];
                if( result < 0 ) {
                    output[co][h][w] = 0.0f;
                }
                else if( result > 6 ) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw4_conv_1x1_output", "w");
    for(int i = 0; i < 384; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw4_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}


void dw5_conv_3x3(
            float input[384][20][40],
            float weight[384][3][3],
            float bias[384],
            float output[384][20][40],
            //float mask[384][20][40]
            int mask_boundary[4]
            )
{
    //cout << "dw5_conv_3x3..." << endl;

    for (int co = 0; co < 384; co++) {
        for (int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for (int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for (int m = 0; m < 3; m++) {
                    for (int n = 0; n < 3; n++) {

                        sum += weight[co][m][n] *
                            ((h + m - 1 >= mask_boundary[0] && w + n - 1 >= mask_boundary[2] && h + m - 1 < mask_boundary[1] && w + n - 1 < mask_boundary[3]) ? input[co][h + m - 1][w + n - 1] : 0);
                    }
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw5_conv_3x3_output", "w");
    for(int i = 0; i < 384; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw5_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}



void dw5_conv_1x1(
            float input[384][20][40],
            float weight[512][384],
            float bias[512],
            float output[512][20][40],
            //float mask[384][20][40]
            int mask_boundary[4]
            )
{
    //cout << "dw5_conv_1x1..." << endl;

    for (int co = 0; co < 512; co++) {
        for (int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for (int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for (int ci = 0; ci < 384; ci++) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw5_conv_1x1_output", "w");
    for(int i = 0; i < 512; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw5_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}



void concat(
            float dw3_conv_1x1_output[192][40][80],
            float dw5_conv_1x1_output[512][20][40],
            float dw6_conv_3x3_input[1280][20][40]
            )
{

    //cout << "cat dw3(ch:192 -> 768) and dw5(ch:512)..." << endl;

    // First reorder dw3_conv_1x1_output: ch192 -> ch768
    for(int co = 0; co < 192; co++) {
        for(int h = 0; h < 40; h+=4) {
            for(int w = 0; w < 80; w+=4) {
                dw6_conv_3x3_input[co][h/2][w/2]       = dw3_conv_1x1_output[co][h][w];
                dw6_conv_3x3_input[co][h/2+1][w/2]     = dw3_conv_1x1_output[co][h+2][w];
                dw6_conv_3x3_input[co][h/2][w/2+1]     = dw3_conv_1x1_output[co][h][w+2];
                dw6_conv_3x3_input[co][h/2+1][w/2+1]   = dw3_conv_1x1_output[co][h+2][w+2];

                dw6_conv_3x3_input[co+192*1][h/2][w/2]     = dw3_conv_1x1_output[co][h][w+1];
                dw6_conv_3x3_input[co+192*1][h/2+1][w/2]   = dw3_conv_1x1_output[co][h+2][w+1];
                dw6_conv_3x3_input[co+192*1][h/2][w/2+1]   = dw3_conv_1x1_output[co][h][w+3];
                dw6_conv_3x3_input[co+192*1][h/2+1][w/2+1] = dw3_conv_1x1_output[co][h+2][w+3];

                dw6_conv_3x3_input[co+192*2][h/2][w/2]     = dw3_conv_1x1_output[co][h+1][w];
                dw6_conv_3x3_input[co+192*2][h/2+1][w/2]   = dw3_conv_1x1_output[co][h+3][w];
                dw6_conv_3x3_input[co+192*2][h/2][w/2+1]   = dw3_conv_1x1_output[co][h+1][w+2];
                dw6_conv_3x3_input[co+192*2][h/2+1][w/2+1] = dw3_conv_1x1_output[co][h+3][w+2];

                dw6_conv_3x3_input[co+192*3][h/2][w/2]     = dw3_conv_1x1_output[co][h+1][w+1];
                dw6_conv_3x3_input[co+192*3][h/2+1][w/2]   = dw3_conv_1x1_output[co][h+3][w+1];
                dw6_conv_3x3_input[co+192*3][h/2][w/2+1]   = dw3_conv_1x1_output[co][h+1][w+3];
                dw6_conv_3x3_input[co+192*3][h/2+1][w/2+1] = dw3_conv_1x1_output[co][h+3][w+3];
            }
        }
    }

    for(int co = 0; co < 512; co++) {
        for(int h = 0; h < 20; h++) {
            for(int w = 0; w < 40; w++) {
                dw6_conv_3x3_input[co + 768][h][w] = dw5_conv_1x1_output[co][h][w];
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw6_conv_3x3_input", "w");
    for(int i = 0; i < 1280; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw6_conv_3x3_input[%d][%d][%d] = %.3f\n", i, j, k, dw6_conv_3x3_input[i][j][k]);
            }
        }
    }
    fclose(fo);


    fo = fopen("dw3_conv_3x3_output_reorg_before", "w");
    for(int i = 0; i < 192; i++) {
        for(int j = 0; j < 40; j++) {
            for(int k = 0; k < 80; k ++) {
                fprintf(fo, "dw3_conv_3x3_output_reorg_before[%d][%d][%d] = %.3f\n", i, j, k, dw3_conv_1x1_output[i][j][k]);
            }
        }
    }
    fclose(fo);

    fo = fopen("dw3_conv_3x3_output_reorg_after", "w");
    for(int i = 0; i < 768; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw3_conv_3x3_output_reorg_after[%d][%d][%d] = %.3f\n", i, j, k, dw6_conv_3x3_input[i][j][k]);
            }
        }
    }
    fclose(fo);


#endif

}




void concat_reo(
            float dw3_conv_1x1_output[192][40][80],
            float dw5_conv_1x1_output[512][20][40],
            float dw6_conv_3x3_input[1280][20][40]
            )
{

    //cout << "cat dw3(ch:192 -> 768) and dw5(ch:512)..." << endl;

    // First reorder dw3_conv_1x1_output: ch192 -> ch768
    for(int co = 0; co < 192; co++) {
        for(int h = 0; h < 40; h+=4) {
            for(int w = 0; w < 80; w+=4) {
                dw6_conv_3x3_input[co*4][h/2  ][w/2  ]     = dw3_conv_1x1_output[co][h  ][w  ];
                dw6_conv_3x3_input[co*4][h/2+1][w/2  ]     = dw3_conv_1x1_output[co][h+2][w  ];
                dw6_conv_3x3_input[co*4][h/2  ][w/2+1]     = dw3_conv_1x1_output[co][h  ][w+2];
                dw6_conv_3x3_input[co*4][h/2+1][w/2+1]     = dw3_conv_1x1_output[co][h+2][w+2];

                dw6_conv_3x3_input[co*4+1][h/2  ][w/2  ]   = dw3_conv_1x1_output[co][h  ][w+1];
                dw6_conv_3x3_input[co*4+1][h/2+1][w/2  ]   = dw3_conv_1x1_output[co][h+2][w+1];
                dw6_conv_3x3_input[co*4+1][h/2  ][w/2+1]   = dw3_conv_1x1_output[co][h  ][w+3];
                dw6_conv_3x3_input[co*4+1][h/2+1][w/2+1]   = dw3_conv_1x1_output[co][h+2][w+3];

                dw6_conv_3x3_input[co*4+2][h/2  ][w/2  ]   = dw3_conv_1x1_output[co][h+1][w  ];
                dw6_conv_3x3_input[co*4+2][h/2+1][w/2  ]   = dw3_conv_1x1_output[co][h+3][w  ];
                dw6_conv_3x3_input[co*4+2][h/2  ][w/2+1]   = dw3_conv_1x1_output[co][h+1][w+2];
                dw6_conv_3x3_input[co*4+2][h/2+1][w/2+1]   = dw3_conv_1x1_output[co][h+3][w+2];

                dw6_conv_3x3_input[co*4+3][h/2  ][w/2  ]   = dw3_conv_1x1_output[co][h+1][w+1];
                dw6_conv_3x3_input[co*4+3][h/2+1][w/2  ]   = dw3_conv_1x1_output[co][h+3][w+1];
                dw6_conv_3x3_input[co*4+3][h/2  ][w/2+1]   = dw3_conv_1x1_output[co][h+1][w+3];
                dw6_conv_3x3_input[co*4+3][h/2+1][w/2+1]   = dw3_conv_1x1_output[co][h+3][w+3];
            }
        }
    }

    for(int co = 0; co < 512; co++) {
        for(int h = 0; h < 20; h++) {
            for(int w = 0; w < 40; w++) {
                dw6_conv_3x3_input[co + 768][h][w] = dw5_conv_1x1_output[co][h][w];
            }
        }
    }

#ifdef DEBUG
    fo = fopen("dw6_conv_3x3_input_reo", "w");
    for(int i = 0; i < 1280; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw6_conv_3x3_input_reo[%d][%d][%d] = %.3f\n", i, j, k, dw6_conv_3x3_input[i][j][k]);
            }
        }
    }
    fclose(fo);

#endif

}



void dw6_conv_3x3(
            float input[1280][20][40],
            float weight[1280][3][3],
            float bias[1280],
            float output[1280][20][40],
            //float mask[1280][20][40]
            int mask_boundary[4]
            )
{
    //cout << "dw6_conv_3x3..." << endl;

    for (int co = 0; co < 1280; co++) {
        for (int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for (int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for (int m = 0; m < 3; m++) {
                    for (int n = 0; n < 3; n++) {

                        sum += weight[co][m][n] *
                            ((h + m - 1 >= mask_boundary[0] && w + n - 1 >= mask_boundary[2] && h + m - 1 < mask_boundary[1] && w + n - 1 < mask_boundary[3]) ? input[co][h + m - 1][w + n - 1]  : 0);
                    }
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw6_conv_3x3_output", "w");
    for(int i = 0; i < 1280; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw6_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}



void dw6_conv_3x3_reo(
            float input[1280][20][40],
            float weight[1280][3][3],
            float bias[1280],
            float output[1280][20][40],
            int mask_boundary[4]
            )
{
    //cout << "dw6_conv_3x3..." << endl;

    for (int co = 0; co < 1280; co++) {
        for (int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for (int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for (int m = 0; m < 3; m++) {
                    for (int n = 0; n < 3; n++) {

                        sum += weight[co][m][n] *
                            ((h + m - 1 >= mask_boundary[0] && w + n - 1 >= mask_boundary[2] && h + m - 1 < mask_boundary[1] && w + n - 1 < mask_boundary[3]) ? input[co][h + m - 1][w + n - 1]  : 0);
                    }
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw6_conv_3x3_output_reo", "w");
    for(int i = 0; i < 1280; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw6_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}



void dw6_conv_1x1(
            float input[1280][20][40],
            float weight[96][1280],
            float bias[96],
            float output[96][20][40],
            //float mask[1280][20][40]
            int mask_boundary[4]
            )
{
    //cout << "dw6_conv_1x1..." << endl;

    for (int co = 0; co < 96; co++) {
        for (int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for (int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for (int ci = 0; ci < 1280; ci++) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw6_conv_1x1_output", "w");
    for(int i = 0; i < 96; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw6_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}




void dw6_conv_1x1_reo(
            float input[1280][20][40],
            float weight[96][1280],
            float bias[96],
            float output[96][20][40],
            int mask_boundary[4]
            )
{
    //cout << "dw6_conv_1x1..." << endl;

    for (int co = 0; co < 96; co++) {
        for (int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for (int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for (int ci = 0; ci < 1280; ci++) {
                    sum += weight[co][ci] * input[ci][h][w] ;
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
#ifdef DEBUG
    fo = fopen("dw6_conv_1x1_output_reo", "w");
    for(int i = 0; i < 96; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "dw6_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}




void pw7_conv_1x1(
            float input[96][20][40],
            float weight[10][96],
            
            float output[10][20][40],
            //float mask[96][20][40]
            int mask_boundary[4]
            )
{
    //cout << "pw7_conv_1x1..." << endl;

    for(int co = 0; co < 10; co++) {
        for(int h = mask_boundary[0]; h < mask_boundary[1]; h++) {
            for(int w = mask_boundary[2]; w < mask_boundary[3]; w++) {
                float sum = 0;

                for(int ci = 0; ci < 96; ci++ ) {
                    sum += weight[co][ci] * input[ci][h][w];
                }
                output[co][h][w] = sum;
            }
        }
    }

#ifdef DEBUG
    fo = fopen("pw7_conv_1x1_output", "w");
    for(int i = 0; i < 10; i++) {
        for(int j = 0; j < 20; j++) {
            for(int k = 0; k < 40; k ++) {
                fprintf(fo, "pw7_conv_1x1_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
#endif
}

void channel_shuffle(float input[192][20][40], float output[192][20][40])
{
    for (int i = 0; i < 6; i++) {
        for (int c = 0; c < 32; c++) {
            for (int h = 0; h < 20; h++) {
                for (int w = 0; w < 40; w++) {
                    output[i * 32 + c][h][w] = input[6 * c + i][h][w];
                }
            }
        }
            
    }
        
}

void new_branch_conv_5x5_shuffle(
    float input[192][20][40],
    float weight[32][6][5][5],
    float bias[32],
    float output[32][20][40]
) {
    for (int co = 0; co < 32; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {
                float sum = 0;
                for (int ci = 0; ci < 6; ci++) {



                    for (int m = 0; m < 5; m++) {
                        for (int n = 0; n < 5; n++) {

                            sum += weight[co][ci][m][n] *
                                ((h + m - 2 >= 0 && w + n - 2 >= 0 && h + m - 2 < 20 && w + n - 2 < 40) ? input[ci * 32 + co][h + m - 2][w + n - 2] : 0);
                        }
                    }
                }
                float result = sum + bias[co];
                if (result < 0) {
                    output[co][h][w] = 0.0f;
                }
                else if (result > 6) {
                    output[co][h][w] = 6.0f;
                }
                else {
                    output[co][h][w] = result;
                }
            }
        }
    }
}


void new_branch_conv_5x5(
    float input[192][20][40],
    float weight[32][6][5][5],
    float bias[32],
    float output[32][20][40]
    //float mask[96][20][40]
)

{
    //cout << "dw6_conv_3x3..." << endl;
    
        for (int co = 0; co < 32; co++) {
            for (int h = 0; h < 20; h++) {
                for (int w = 0; w < 40; w++) {
                    float sum = 0;
                    for (int ci = 0; ci < 6; ci++) {



                        for (int m = 0; m < 5; m++) {
                            for (int n = 0; n < 5; n++) {

                                sum += weight[co][ci][m][n] *
                                    ((h + m - 2 >= 0 && w + n - 2 >= 0 && h + m - 2 < 20 && w + n - 2 < 40) ? input[co*6+ci][h + m - 2][w + n - 2] : 0);
                            }
                        }
                    }
                    float result = sum + bias[co];
                    if (result < 0) {
                        output[co][h][w] = 0.0f;
                    }
                    else if (result > 6) {
                        output[co][h][w] = 6.0f;
                    }
                    else {
                        output[co][h][w] = result;
                    }
                }
            }
        }
        fo = fopen("conv5x5_weight", "w");
        for (int co = 0; co < 32; co++) {
            for (int ci = 0; ci < 6; ci++) {
                for (int h = 0; h < 5; h++) {
                    for (int w = 0; w < 5; w++) {
                        {
                            fprintf(fo, "conv_5x5_weight[%d][%d][%d][%d] = %.3f\n", co,ci, h, w, weight[co][ci][h][w]);
                        }
                    }
                }
            }
            
        }
        
        fclose(fo);

        fo = fopen("conv5x5_input", "w");
        for (int co = 0; co < 192; co++) {
            
                for (int h = 0; h < 20; h++) {
                    for (int w = 0; w < 40; w++) {
                        {
                            fprintf(fo, "conv_5x5_input[%d][%d][%d] = %.3f\n", co, h, w, input[co][h][w]);
                        }
                    }
                }
            

        }

        fclose(fo);

        fo = fopen("conv5x5_output", "w");
        for (int co = 0; co < 32; co++) {

            for (int h = 0; h < 20; h++) {
                for (int w = 0; w < 40; w++) {
                    {
                        fprintf(fo, "conv_5x5_output[%d][%d][%d] = %.3f\n", co, h, w, output[co][h][w]);
                    }
                }
            }


        }

        fclose(fo);
    
    
}

void new_branch_conv_3x3_1(
    float input[32][5][5],
    float weight[1][32][3][3],
    float output[1][5][5]
    //float mask[96][20][40]
)
{
    //cout << "dw6_conv_3x3..." << endl;

    for (int co = 0; co < 1; co++) {
        for (int h = 0; h < 5; h++) {
            for (int w = 0; w < 5; w++) {
                float sum = 0;
                for (int ci = 0; ci < 32; ci++) {


                    for (int m = 0; m < 3; m++) {
                        for (int n = 0; n < 3; n++) {

                            sum += weight[co][ci][m][n] *
                                ((h + m - 1 >= 0 && w + n - 1 >= 0 && h + m - 1 < 5 && w + n - 1 < 5) ? input[ci][h + m - 1][w + n - 1] : 0);
                        }
                    }
                }

            
                float result = sum;
                output[co][h][w] = 1.0f / (1.0f + exp(-result));
                
            }
        }
    }

    fo = fopen("new_branch_conv_3x3_output", "w");
    for (int i = 0; i < 1; i++) {
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 8; k++) {
                fprintf(fo, "new_branch_conv_3x3_output[%d][%d][%d] = %.3f\n", i, j, k, output[i][j][k]);
            }
        }
    }
    fclose(fo);
}

void new_branch_5x5_max_pool(
    float input[192][40][80],
    float output[192][40][40]
    //float mask[192][40][80]
)
{
    //cout << "dw3_max_pool..." << endl;

    for (int co = 0; co < 192; co++) {
        for (int h = 0; h < 40; h++) {
            for (int w = 0; w < 40; w++) {

                output[co][h][w] = max_2(
                    input[co][h][w * 2],
                    input[co][h][w * 2 + 1]
                    
                );
            }
        }
    }
}
void new_branch_3x3_max_pool1(
    float input[32][20][40],
    float output[32][10][20]
)
{
    //cout << "dw2_max_pool..." << endl;

    for (int co = 0; co < 32; co++) {
        for (int h = 0; h < 10; h++) {
            for (int w = 0; w < 20; w++) {

                output[co][h][w] = max_4(
                    input[co][h * 2][w * 2],
                    input[co][h * 2 + 1][w * 2],
                    input[co][h * 2][w * 2 + 1],
                    input[co][h * 2 + 1][w * 2 + 1]
                );
            }
        }
    }
}

void new_branch_3x3_max_pool2(
    float input[32][10][20],
    float output[32][5][10]
)
{
    //cout << "dw2_max_pool..." << endl;

    for (int co = 0; co < 32; co++) {
        for (int h = 0; h < 5; h++) {
            for (int w = 0; w < 10; w++) {

                output[co][h][w] = max_4(
                    input[co][h * 2][w * 2],
                    input[co][h * 2 + 1][w * 2],
                    input[co][h * 2][w * 2 + 1],
                    input[co][h * 2 + 1][w * 2 + 1]
                );
            }
        }
    }
}

void new_branch_3x3_max_pool3(
    float input[32][5][10],
    float output[32][5][5]
)
{
    //cout << "dw2_max_pool..." << endl;

    for (int co = 0; co < 32; co++) {
        for (int h = 0; h < 5; h++) {
            for (int w = 0; w < 5; w++) {

                output[co][h][w] = max_2(
                    input[co][h][w * 2],
                    
                    input[co][h][w * 2 + 1]
                   
                );
            }
        }
    }
}





void new_branch_3x3_max_pool(
    float input[32][10][20],
    float output[32][5][10]
    //float mask[192][40][80]
)
{
    //cout << "dw3_max_pool..." << endl;

    for (int co = 0; co < 32; co++) {
        for (int h = 0; h < 4; h++) {
            for (int w = 0; w < 8; w++) {
                float tmp1, tmp2, tmp3, tmp4, tmp5;

                tmp1 = max_5(
                    input[co][h * 5][w * 5],
                    input[co][h * 5][w * 5 + 1],
                    input[co][h * 5][w * 5 + 2],
                    input[co][h * 5][w * 5 + 3],
                    input[co][h * 5][w * 5 + 4]

                );

                tmp2 = max_5(
                    input[co][h * 5 +1][w * 5],
                    input[co][h * 5 +1][w * 5 + 1],
                    input[co][h * 5 +1][w * 5 + 2],
                    input[co][h * 5 +1][w * 5 + 3],
                    input[co][h * 5 +1][w * 5 + 4]

                );

                tmp3 = max_5(
                    input[co][h * 5 + 2][w * 5],
                    input[co][h * 5 + 2][w * 5 + 1],
                    input[co][h * 5 + 2][w * 5 + 2],
                    input[co][h * 5 + 2][w * 5 + 3],
                    input[co][h * 5 + 2][w * 5 + 4]

                );

                tmp4 = max_5(
                    input[co][h * 5 + 3][w * 5],
                    input[co][h * 5 + 3][w * 5 + 1],
                    input[co][h * 5 + 3][w * 5 + 2],
                    input[co][h * 5 + 3][w * 5 + 3],
                    input[co][h * 5 + 3][w * 5 + 4]

                );

                tmp5 = max_5(
                    input[co][h * 5 + 4][w * 5],
                    input[co][h * 5 + 4][w * 5 + 1],
                    input[co][h * 5 + 4][w * 5 + 2],
                    input[co][h * 5 + 4][w * 5 + 3],
                    input[co][h * 5 + 4][w * 5 + 4]

                );
                output[co][h][w] = max_5(tmp1, tmp2, tmp3, tmp4, tmp5);
            }
        }
    }
}

void upsample_mask_192_20_40(float mask[1][5][5] , float output_mask[192][20][40])
{
    for (int co = 0; co < 192; co++) {
        for (int h = 0; h < 5; h++) {
            for (int w = 0; w < 5; w++) {
                for (int i = 0; i < 4; i++) {
                    for (int j = 0; j < 8; j++) {
                        //output_mask[co][h][w] = mask[1][h][w];
                        output_mask[co][h*4 + i][w*8 + j] = mask[0][h][w];
                    }
                }
                
            }
        }
    }
}

void downsample_mask_192_20_40(float mask[192][40][80], float output_mask[192][20][40])
{
    for (int co = 0; co < 192; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {
                output_mask[co][h][w] = max_4(
                    mask[co][h * 2][w * 2],
                    mask[co][h * 2 + 1][w * 2],
                    mask[co][h * 2][w * 2 + 1],
                    mask[co][h * 2 + 1][w * 2 + 1]
                );

            }
        }
    }
}

void upsample_mask_384_20_40(float mask[192][20][40], float output_mask[384][20][40])
{
    for (int co = 0; co < 384; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {
                output_mask[co][h][w] = mask[0][h][w];

            }
        }
    }
}

void upsample_mask_512_20_40(float mask[384][20][40], float output_mask[512][20][40])
{
    for (int co = 0; co < 512; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {
                output_mask[co][h][w] = mask[0][h][w];

            }
        }
    }
}

void upsample_mask_1280_20_40(float mask[512][20][40], float output_mask[1280][20][40])
{
    for (int co = 0; co < 1280; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {
                output_mask[co][h][w] = mask[0][h][w];

            }
        }
    }
}

void downsample_mask_96_20_40(float mask[1280][20][40], float output_mask[96][20][40])
{
    for (int co = 0; co < 96; co++) {
        for (int h = 0; h < 20; h++) {
            for (int w = 0; w < 40; w++) {
                output_mask[co][h][w] = mask[0][h][w];

            }
        }
    }
}

void generate_mask(float mask[1][5][5], int xmin[1], int xmax[1], int ymin[1], int ymax[1], int index[1]) {
    xmin[0] = 4;
    xmax[0] = 0;
    ymin[0] = 4;
    ymax[0] = 0;
    index[0] = 0;

    for (int m = 0; m < 5; m++) {
        for (int n = 0; n < 5; n++) {
            if (mask[0][m][n] > 0.3) {
                index[0]++;
                if (m  < xmin[0]) xmin[0] = m ;
                if (n  < ymin[0]) ymin[0] = n ;
                if (m  > xmax[0]) xmax[0] = m ;
                if (n  > ymax[0]) ymax[0] = n ;
            }
        }
    }
}
    


void compute_bounding_box( float pw7_conv_1x1_out[10][20][40], int predict_box[5],int mask_boundary[4])
{
    int batch_size = 1;
    int num_anchors = 2;
    int h = 20;
    int w = 40;

    float box[4] = {1.4940052559648322, 2.3598481287086823, 4.0113013115312155, 5.760873975661669};

    float conf_thresh = 0.0;
    int conf_j = 0;
    int conf_m = 0;
    int conf_n = 0;


    float output[10][20][40];
    for(int i = 0; i < 10; i++) {
    	for(int j = mask_boundary[0]; j < mask_boundary[1]; j++) {
    		for(int k = mask_boundary[2]; k < mask_boundary[3]; k++) {
    			output[i][j][k] = pw7_conv_1x1_out[i][j][k];
    		}
    	}
    }



    //preprocessing anchor boxes xs and ys
    for(int j = 0;j < num_anchors;j++){
        for(int m = mask_boundary[0]; m < mask_boundary[1]; m++){
            for(int n = mask_boundary[2]; n < mask_boundary[3]; n++){
                output[j*5][m][n] = 1/(1+exp(-output[j*5][m][n]))+n;
            }
        }
    }

    for(int j = 0;j < num_anchors;j++){
        for(int m = mask_boundary[0]; m < mask_boundary[1]; m++){
            for(int n = mask_boundary[2]; n < mask_boundary[3]; n++){
                output[j*5+1][m][n] = 1/(1+exp(-output[j*5+1][m][n]))+m;
            }
        }
    }
    //preprocessing anchor boxes ws and hs
    for(int j = 0;j < num_anchors;j++){
        for(int m = mask_boundary[0]; m < mask_boundary[1]; m++){
            for(int n = mask_boundary[2]; n < mask_boundary[3]; n++){
                output[j*5+2][m][n] = exp(output[j*5+2][m][n])*box[j*2];
            }
        }
    }
    for(int j = 0;j < num_anchors;j++){
        for(int m = mask_boundary[0]; m < mask_boundary[1]; m++){
            for(int n = mask_boundary[2]; n < mask_boundary[3]; n++){
                output[j*5+3][m][n] = exp(output[j*5+3][m][n])*box[j*2+1];
            }
        }
    }
    //preprocessing anchor boxes det_conf
    for(int j = 0;j < num_anchors;j++){
        for(int m = mask_boundary[0]; m < mask_boundary[1]; m++){
            for(int n = mask_boundary[2]; n < mask_boundary[3]; n++){
                output[j*5+4][m][n] = 1/(1+exp(-output[j*5+4][m][n]));
            }
        }
    }

    //find the maximum num
    for(int j = 0;j < num_anchors;j++){
        for(int m = mask_boundary[0]; m < mask_boundary[1]; m++){
            for(int n = mask_boundary[2]; n < mask_boundary[3]; n++){
                if(output[j*5+4][m][n] > conf_thresh){
                    conf_thresh = output[j*5+4][m][n];
                    conf_j = j;
                    conf_m = m;
                    conf_n = n;
                }
            }
        }
    }

    //calculate the output

    float bbox[5] = {0.0};
    bbox[0] = output[conf_j*5+0][conf_m][conf_n]/w;
    bbox[1] = output[conf_j*5+1][conf_m][conf_n]/h;
    bbox[2] = output[conf_j*5+2][conf_m][conf_n]/w;
    bbox[3] = output[conf_j*5+3][conf_m][conf_n]/h;
    bbox[4] = output[conf_j*5+4][conf_m][conf_n];

     for(int i = 0; i < 5; i++){
         printf("%f ", bbox[i]);
     }
     printf("\n");

    predict_box[0] = (unsigned int)(((bbox[0] - bbox[2]/2.0)) * 640);
    predict_box[1] = (unsigned int)(((bbox[1] - bbox[3]/2.0)) * 360);
    predict_box[2] = (unsigned int)(((bbox[0] + bbox[2]/2.0)) * 640);
    predict_box[3] = (unsigned int)(((bbox[1] + bbox[3]/2.0)) * 360);

}





int golden_model()
{

    // reorder dw6_conv_3x3_weight and dw6_conv_3x3_bias
    for(int ch = 0; ch < 768; ch += 4) {
        dw6_conv_3x3_bias_reo[ch] = dw6_conv_3x3_bias[ ch / 4];
        dw6_conv_3x3_bias_reo[ch + 1] = dw6_conv_3x3_bias[ ch / 4 + 192];
        dw6_conv_3x3_bias_reo[ch + 2] = dw6_conv_3x3_bias[ ch / 4 + 192 * 2];
        dw6_conv_3x3_bias_reo[ch + 3] = dw6_conv_3x3_bias[ ch / 4 + 192 * 3];

        for(int m = 0; m < 3; m++) {
            for(int n = 0; n < 3; n++) {
                dw6_conv_3x3_weight_reo[ch][m][n] = dw6_conv_3x3_weight[ ch / 4 ][m][n];
                dw6_conv_3x3_weight_reo[ch + 1][m][n] = dw6_conv_3x3_weight[ ch / 4 + 192][m][n];
                dw6_conv_3x3_weight_reo[ch + 2][m][n] = dw6_conv_3x3_weight[ ch / 4 + 192 * 2][m][n];
                dw6_conv_3x3_weight_reo[ch + 3][m][n] = dw6_conv_3x3_weight[ ch / 4 + 192 * 3][m][n];
            }
        }

    }
    for(int ch = 768; ch < 1280; ch++) {
        dw6_conv_3x3_bias_reo[ch] = dw6_conv_3x3_bias[ch];

        for(int m = 0; m < 3; m++) {
            for(int n = 0; n < 3; n++) {
                dw6_conv_3x3_weight_reo[ch][m][n] = dw6_conv_3x3_weight[ch][m][n];
            }
        }
    }


    // reorder dw6_conv_1x1_weight and dw6_conv_1x1_bias

    for(int ci = 0; ci < 768; ci += 4) {
        for(int co = 0; co < 96; co++) {
            dw6_conv_1x1_weight_reo[co][ci] = dw6_conv_1x1_weight[co][ ci / 4 ];
            dw6_conv_1x1_weight_reo[co][ci + 1] = dw6_conv_1x1_weight[co][ ci / 4 + 192];
            dw6_conv_1x1_weight_reo[co][ci + 2] = dw6_conv_1x1_weight[co][ ci / 4 + 192 * 2];
            dw6_conv_1x1_weight_reo[co][ci + 3] = dw6_conv_1x1_weight[co][ ci / 4 + 192 * 3];
        }
    }

    for(int ci = 768; ci < 1280; ci++) {
        for(int co = 0; co < 96; co++) {
            dw6_conv_1x1_weight_reo[co][ci] = dw6_conv_1x1_weight[co][ci];
        }
    }

    for(int co = 0; co < 96; co++) {
        dw6_conv_1x1_bias_reo[co] = dw6_conv_1x1_bias[co];
    }

    
	dw1_conv_3x3(image, dw1_conv_3x3_weight, dw1_conv_3x3_bias, dw1_conv_3x3_output);
	dw1_conv_1x1(dw1_conv_3x3_output, dw1_conv_1x1_weight, dw1_conv_1x1_bias, dw1_conv_1x1_output);
	dw1_max_pool(dw1_conv_1x1_output, dw1_max_pool_output);

	dw2_conv_3x3(dw1_max_pool_output, dw2_conv_3x3_weight, dw2_conv_3x3_bias, dw2_conv_3x3_output);
	dw2_conv_1x1(dw2_conv_3x3_output, dw2_conv_1x1_weight, dw2_conv_1x1_bias, dw2_conv_1x1_output);
	dw2_max_pool(dw2_conv_1x1_output, dw2_max_pool_output);

	dw3_conv_3x3(dw2_max_pool_output, dw3_conv_3x3_weight, dw3_conv_3x3_bias, dw3_conv_3x3_output);
	dw3_conv_1x1(dw3_conv_3x3_output, dw3_conv_1x1_weight, dw3_conv_1x1_bias, dw3_conv_1x1_output);
    dw3_max_pool(dw3_conv_1x1_output, dw3_max_pool_output);


    channel_shuffle(dw3_max_pool_output, dw3_max_pool_output_reorg);
    new_branch_conv_5x5_shuffle(dw3_max_pool_output_reorg, new_branch_conv_5x5_weight, new_branch_conv_5x5_bias, new_branch_conv_5x5_output);

    fo = fopen("new_branch_conv_5x5_output", "w");

    for (int ci = 0; ci < 32; ci++) {
        for (int i = 0; i < 20; i++) {
            for (int j = 0; j < 40; j++) {

                fprintf(fo, "new_branch_conv_5x5_output[%d][%d][%d]= %.3f\n", ci, i, j, (float)new_branch_conv_5x5_output[ci][i][j]);

            }
        }
    }
    new_branch_3x3_max_pool1(new_branch_conv_5x5_output, new_branch_conv_3x3_max_pool_output1);
    fo = fopen("new_branch_conv_3x3_max_pool_output1", "w");

    for (int ci = 0; ci < 32; ci++) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 20; j++) {

                fprintf(fo, "new_branch_conv_3x3_max_pool_output1[%d][%d][%d]= %.3f\n", ci, i, j, (float)new_branch_conv_3x3_max_pool_output1[ci][i][j]);

            }
        }
    }

    new_branch_3x3_max_pool2(new_branch_conv_3x3_max_pool_output1, new_branch_conv_3x3_max_pool_output2);
    new_branch_3x3_max_pool3(new_branch_conv_3x3_max_pool_output2, new_branch_conv_3x3_max_pool_output3);
    new_branch_conv_3x3_1(new_branch_conv_3x3_max_pool_output3, new_branch_conv_3x3_weight, mask_temp);

    fo = fopen("mask_temp", "w");
    for (int h = 0; h < 5; h++) {
        for (int w = 0; w < 5; w++) {
            {
                fprintf(fo, "mask_temp[%d][%d][%d] = %.3f\n", 1, h, w, mask_temp[0][h][w]);
            }
        }
    }
    fclose(fo);

    mask_threshold(mask_temp, mask);
    generate_mask(mask, xmin, xmax, ymin, ymax, my_index);

    if (my_index[0] == 0) {
        xmin[0] = 0;
        xmax[0] = 4;
        ymin[0] = 0;
        ymax[0] = 4;

    }

    mask_boundary[0] = xmin[0] * 4;
    mask_boundary[1] = (xmax[0] + 1) * 4;
    mask_boundary[2] = ymin[0] * 8;
    mask_boundary[3] = (ymax[0] + 1) * 8;


    upsample_mask_192_20_40(mask, dw4__conv_3x3_mask);
    upsample_mask_384_20_40(dw4__conv_3x3_mask, dw4__conv_1x1_mask);
    upsample_mask_512_20_40(dw4__conv_1x1_mask, dw5__conv_1x1_mask);
    upsample_mask_1280_20_40(dw5__conv_1x1_mask, dw6_conv_3x3_mask);
    downsample_mask_96_20_40(dw6_conv_3x3_mask, dw6_conv_1x1_mask);

	//

	

	dw4_conv_3x3(dw3_max_pool_output, dw4_conv_3x3_weight, dw4_conv_3x3_bias, dw4_conv_3x3_output, mask_boundary);
	dw4_conv_1x1(dw4_conv_3x3_output, dw4_conv_1x1_weight, dw4_conv_1x1_bias, dw4_conv_1x1_output, mask_boundary);

	dw5_conv_3x3(dw4_conv_1x1_output, dw5_conv_3x3_weight, dw5_conv_3x3_bias, dw5_conv_3x3_output, mask_boundary);
	dw5_conv_1x1(dw5_conv_3x3_output, dw5_conv_1x1_weight, dw5_conv_1x1_bias, dw5_conv_1x1_output, mask_boundary);

	concat(dw3_conv_1x1_output, dw5_conv_1x1_output, dw6_conv_3x3_input);
	concat_reo(dw3_conv_1x1_output, dw5_conv_1x1_output, dw6_conv_3x3_input_reo);

	dw6_conv_3x3(dw6_conv_3x3_input,  dw6_conv_3x3_weight, dw6_conv_3x3_bias, dw6_conv_3x3_output, mask_boundary);
	dw6_conv_3x3_reo(dw6_conv_3x3_input_reo,  dw6_conv_3x3_weight_reo, dw6_conv_3x3_bias_reo, dw6_conv_3x3_output_reo, mask_boundary);

	dw6_conv_1x1(dw6_conv_3x3_output, dw6_conv_1x1_weight, dw6_conv_1x1_bias, dw6_conv_1x1_output, mask_boundary);
	dw6_conv_1x1_reo(dw6_conv_3x3_output_reo, dw6_conv_1x1_weight_reo, dw6_conv_1x1_bias_reo, dw6_conv_1x1_output_reo, mask_boundary);

	pw7_conv_1x1(dw6_conv_1x1_output, pw7_conv_1x1_weight, pw7_conv_1x1_output, mask_boundary);

	int predict_box[5];
	compute_bounding_box(pw7_conv_1x1_output, predict_box ,mask_boundary);
    printf("%d %d %d %d\n", predict_box[0], predict_box[1], predict_box[2], predict_box[3]);

    return 0;
}



