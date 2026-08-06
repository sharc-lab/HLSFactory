//--------------------------------------------------------------------------
// Perform software-level convolution to ensure functional correctness.
//--------------------------------------------------------------------------
#include "conv.h"
#include <iostream>
#include <math.h>

void model_conv_bn (
    fm_t *input_feature_map,
    wt_t *layer_conv_weights,
    wt_t *layer_bn_weights,
    fm_t *output_feature_map,
    int input_d,
    int input_h,
    int input_w,
    int filter_size,
    int kernel_h,
    int kernel_w
)
{
//--------------------------------------------------------------------------
// Your code for TASK A goes here 
//
// Implement the specified convolution layer at the software level 
// without any hardware level features. Refer to the article on Sharc Lab 
// Knowledge Pool site to learn more.
//
// Hint: Do not forget to add bias and apply ReLU while writing your
//       convolution code!
//--------------------------------------------------------------------------
//Batch Normalization parameters
fm_t *gamma, *beta, *mean, *var, epsilon = 0.001, A, B;
gamma = (fm_t *)malloc(filter_size * sizeof(fm_t));
beta  = (fm_t *)malloc(filter_size * sizeof(fm_t));
mean  = (fm_t *)malloc(filter_size * sizeof(fm_t));
var   = (fm_t *)malloc(filter_size * sizeof(fm_t));
for (int i = 0; i < filter_size; i++)
{
    gamma[i] = layer_bn_weights[(i*4) + 0];
    beta[i]  = layer_bn_weights[(i*4) + 1];
    mean[i]  = layer_bn_weights[(i*4) + 2];
    var[i]   = layer_bn_weights[(i*4) + 3];
}

//Padding of input_feature_map
fm_t ***input_fm_padded;

input_fm_padded = (fm_t ***) malloc(input_d * sizeof(fm_t **));

for(int i = 0;i < input_d; i++)
{
    input_fm_padded[i]=(fm_t **)malloc((input_h + kernel_h - 1) * sizeof(fm_t *));
    for(int j = 0;j < (input_h + kernel_h - 1); j++)
        input_fm_padded[i][j]=(fm_t *)malloc((input_w + kernel_w - 1) * sizeof(fm_t));
}

for (int f = 0; f < input_d; f++)
    for(int i = 0; i < (input_h + kernel_h - 1); i++)
        for(int j = 0; j < (input_w + kernel_w - 1); j++)
        {
            if ((i == 0) || ( j==0 ) || (i == (input_h + kernel_h - 2)) || (j == (input_w + kernel_w - 2)))
                input_fm_padded[f][i][j] = 0;
            else
                input_fm_padded[f][i][j] = input_feature_map[(f*input_h*input_w) + ((i-1)*input_w) + (j-1)];
        }


//Convolution computation
for(volatile int f = 0; f < filter_size; f++)    // Filter Size (Output Depth)
    for(int i = 0; i < input_h; i++)             // Output Height
        for(int j = 0; j < input_w; j++)           // Output Width
        {
            for(int c = 0; c < input_d; c++)        // Input Depth
                for(int kh = 0; kh < kernel_h; kh++)   // Kernel Height
                    for(int kw = 0; kw < kernel_w; kw++) // Kernel Width		
                    {
                        if(c == 0 && kh == 0 && kw == 0)  // Initialize output feature
                            output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]  = input_fm_padded[c][i+kh][j+kw] * layer_conv_weights[(f*input_d*kernel_h*kernel_w)+(c*kernel_h*kernel_w)+(kh*kernel_w)+kw];
                        else                              // Multiple and Accumulate (MAC)
                            output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]  += input_fm_padded[c][i+kh][j+kw] * layer_conv_weights[(f*input_d*kernel_h*kernel_w)+(c*kernel_h*kernel_w)+(kh*kernel_w)+kw];
                    }
            //Batch Normalisation
            //A = gamma[f]/(fm_t)(sqrt(var[f] + epsilon));
            //B = beta[f] - ((gamma[f]*mean[f])/(fm_t)(sqrt(var[f] + epsilon)));
            //output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)] = (A * output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]) + B;
            //Leaky ReLU activation with 0.1 slope as defined by https://github.com/pjreddie/darknet/blob/master/src/activations.h
            if(output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]  < 0)
                output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]  = (fm_t)(0.1)*output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)];
        }
}

void model_conv (
    fm_t *input_feature_map,
    wt_t *layer_conv_weights,
    wt_t *layer_bias,
    fm_t *output_feature_map,
    int input_d,
    int input_h,
    int input_w,
    int filter_size,
    int kernel_h,
    int kernel_w
)
{
//--------------------------------------------------------------------------
// Your code for TASK A goes here 
//
// Implement the specified convolution layer at the software level 
// without any hardware level features. Refer to the article on Sharc Lab 
// Knowledge Pool site to learn more.
//
// Hint: Do not forget to add bias and apply ReLU while writing your
//       convolution code!
//--------------------------------------------------------------------------

//Padding of input_feature_map
fm_t ***input_fm_padded;

input_fm_padded = (fm_t ***) malloc(input_d * sizeof(fm_t **));

for(int i = 0;i < input_d; i++)
{
    input_fm_padded[i]=(fm_t **)malloc((input_h + kernel_h - 1) * sizeof(fm_t *));
    for(int j = 0;j < (input_h + kernel_h - 1); j++)
        input_fm_padded[i][j]=(fm_t *)malloc((input_w + kernel_w - 1) * sizeof(fm_t));
}

for (int f = 0; f < input_d; f++)
    for(int i = 0; i < (input_h + kernel_h - 1); i++)
        for(int j = 0; j < (input_w + kernel_w - 1); j++)
        {
            if ((i == 0) || ( j==0 ) || (i == (input_h + kernel_h - 2)) || (j == (input_w + kernel_w - 2)))
                input_fm_padded[f][i][j] = 0;
            else
                input_fm_padded[f][i][j] = input_feature_map[(f*input_h*input_w) + ((i-1)*input_w) + (j-1)];
        }


//Convolution computation
for(volatile int f = 0; f < filter_size; f++)    // Filter Size (Output Depth)
    for(int i = 0; i < input_h; i++)             // Output Height
        for(int j = 0; j < input_w; j++)           // Output Width
        {
            for(int c = 0; c < input_d; c++)        // Input Depth
                for(int kh = 0; kh < kernel_h; kh++)   // Kernel Height
                    for(int kw = 0; kw < kernel_w; kw++) // Kernel Width		
                    {
                        if(c == 0 && kh == 0 && kw == 0)  // Initialize output feature
                            output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]  = input_fm_padded[c][i+kh][j+kw] * layer_conv_weights[(f*input_d*kernel_h*kernel_w)+(c*kernel_h*kernel_w)+(kh*kernel_w)+kw]  + layer_bias[f];
                        else                              // Multiple and Accumulate (MAC)
                            output_feature_map[(f*input_h*input_w)+(i*input_w)+(j)]  += input_fm_padded[c][i+kh][j+kw] * layer_conv_weights[(f*input_d*kernel_h*kernel_w)+(c*kernel_h*kernel_w)+(kh*kernel_w)+kw];
                    }
        }
}

void model_maxpool2D(
    fm_t *in_buf,
    fm_t *out_buf,
    int input_d,
    int input_h,
    int input_w,
    int stride
)
{
    fm_t max;
    
    int h_out = 0, w_out = 0;
    int output_h, output_w;

    if(stride == 1)
    {
        output_h = input_h;
        output_w = input_w;

        //Padding to be done if stride is 1
        for (int i = 0; i < input_d; i++)
            for (int j = 0; j < input_h; j++)
            {
                in_buf[(i*input_h*input_w)+(j*input_w)+input_w] = 0;
            }

        for (int i = 0; i < input_d; i++)
            for (int j = 0; j < input_w; j++)
            {
                in_buf[(i*input_h*input_w)+(input_h*input_w)+j] = 0;
            }
    }
    else
    {
        output_h = input_h/2;
        output_w = input_w/2;
    }

    for (int d = 0; d < input_d; d++)
        for (int h = 0, h_out = 0; h < input_h; h+=stride, h_out++)
            for (int w = 0, w_out = 0; w < input_w; w+=stride, w_out++)
            {
                max = in_buf[(d*input_h*input_w) + (h*input_w) + w];
                for (int i = 0; i < 2; i++)
                    for (int j = 0; j < 2; j++)
                    {
                        if (in_buf[(d*input_h*input_w) + ((h+i)*input_w) + (w+j)] > max)
                            max = in_buf[(d*input_h*input_w) + ((h+i)*input_w) + (w+j)];
                    }
                out_buf[(d*output_h*output_w) + (h_out*output_w) + w_out] = max;
            }
}

void model_upsample(
    fm_t *in_buf,
    fm_t *out_buf,
    int input_d,
    int input_h,
    int input_w
)
{
    for (int d = 0; d < input_d; d++)
        for(int h = 0; h < input_h; h++)
            for (int w = 0; w < input_w; w++)
            {
                for (int i = 0; i < 2; i++)
                    for(int j = 0; j < 2; j++)
                    {
                        out_buf[(d*(input_h*2)*(input_w*2)) + (((2*h) + i)*(input_w*2)) + ((2*w) + j)] = in_buf[(d*input_h*input_w) + (h*input_w) + w];
                    }
            }
}

fm_t sigmoid(fm_t x)
{
    fm_t output;
    output = 1/(1 + exp(-1*x));
    return output;
}

// YOLO layer
void model_yolo(
    fm_t *in_buf,
    fm_t *out_buf,
    int input_d,
    int input_h,
    int input_w,
    wt_t stride, // for the scaling to represent the output, TODO: update it to a pointer
    wt_t *anchor // for updating the expected width and height of the image, TODO: update to a pointer
    // TODO: update to 255 layers from 85
)
{

    fm_t ***input_dxdy0, ***input_dwdh0, ***input_conf0, ***input_prob0, ***xy_grid0;
    fm_t ***input_dxdy1, ***input_dwdh1, ***input_conf1, ***input_prob1, ***xy_grid1;
    fm_t ***input_dxdy2, ***input_dwdh2, ***input_conf2, ***input_prob2, ***xy_grid2;
    //input_dxdy = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_dxdy0 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_dwdh0 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_conf0 = (fm_t ***) malloc(1 * sizeof(fm_t **));
    input_prob0 = (fm_t ***) malloc(80 * sizeof(fm_t **));

    input_dxdy1 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_dwdh1 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_conf1 = (fm_t ***) malloc(1 * sizeof(fm_t **));
    input_prob1 = (fm_t ***) malloc(80 * sizeof(fm_t **));

    input_dxdy2 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_dwdh2 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    input_conf2 = (fm_t ***) malloc(1 * sizeof(fm_t **));
    input_prob2 = (fm_t ***) malloc(80 * sizeof(fm_t **));


    xy_grid0 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    xy_grid1 = (fm_t ***) malloc(2 * sizeof(fm_t **));
    xy_grid2 = (fm_t ***) malloc(2 * sizeof(fm_t **));

    for(int i = 0;i < 80; i++)
    {
        input_prob0[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_prob1[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_prob2[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        for(int j = 0;j < (input_h); j++)
        {
            input_prob0[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_prob1[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_prob2[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
        }
    }

    for(int i = 0;i < 1; i++)
    {
        input_conf0[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_conf1[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_conf2[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        for(int j = 0;j < input_h; j++)
        {
            input_conf0[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_conf1[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_conf2[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
        }
    }

    for(int i = 0;i < 2; i++)
    {
        input_dxdy0[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_dxdy1[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_dxdy2[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_dwdh0[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_dwdh1[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        input_dwdh2[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        xy_grid0[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        xy_grid1[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        xy_grid2[i]=(fm_t **)malloc((input_h) * sizeof(fm_t *));
        for(int j = 0;j < input_h; j++)
        {
            input_dxdy0[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_dxdy1[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_dxdy2[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_dwdh0[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_dwdh1[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            input_dwdh2[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            xy_grid0[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            xy_grid1[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
            xy_grid2[i][j]=(fm_t *)malloc((input_w) * sizeof(fm_t));
        }
    }

    for (int f = 0; f < 85; f++)
        for(int i = 0; i < (input_h); i++)
            for(int j = 0; j < (input_w); j++)
            {
                if ((f==0) || (f==1))
		        {
                    input_dxdy0[f][i][j] = in_buf[(f*input_h*input_w) + ((i)*input_w) + (j)];
                    if(f==0) xy_grid0[f][i][j] = i;
                    if(f==1) xy_grid0[f][i][j] = j;

                    input_dxdy1[f][i][j] = in_buf[((f+85)*input_h*input_w) + ((i)*input_w) + (j)];
                    if(f==0) xy_grid1[f][i][j] = i;
                    if(f==1) xy_grid1[f][i][j] = j;

                    input_dxdy2[f][i][j] = in_buf[((f+170)*input_h*input_w) + ((i)*input_w) + (j)];
                    if(f==0) xy_grid2[f][i][j] = i;
                    if(f==1) xy_grid2[f][i][j] = j;
		        }
                else if((f==2) || (f==3))
		        {
                    input_dwdh0[f-2][i][j] = in_buf[(f*input_h*input_w) + ((i)*input_w) + (j)];
                    input_dwdh1[f-2][i][j] = in_buf[((f+85)*input_h*input_w) + ((i)*input_w) + (j)];
                    input_dwdh2[f-2][i][j] = in_buf[((f+170)*input_h*input_w) + ((i)*input_w) + (j)];
		        }
                else if(f==4)
		        {
                    input_conf0[f-4][i][j] = in_buf[(f*input_h*input_w) + ((i)*input_w) + (j)];
                    input_conf1[f-4][i][j] = in_buf[((f+85)*input_h*input_w) + ((i)*input_w) + (j)];
                    input_conf2[f-4][i][j] = in_buf[((f+170)*input_h*input_w) + ((i)*input_w) + (j)];
		        }
                else
		        {
                    input_prob0[f-5][i][j] = in_buf[(f*input_h*input_w) + ((i)*input_w) + (j)];
                    input_prob1[f-5][i][j] = in_buf[((f+85)*input_h*input_w) + ((i)*input_w) + (j)];
                    input_prob2[f-5][i][j] = in_buf[((f+170)*input_h*input_w) + ((i)*input_w) + (j)];
		        }
            }

    for (int f = 0; f < 2; f++)
            for(int i = 0; i < (input_h); i++)
                for(int j = 0; j < (input_w); j++)
                {
                    input_dxdy0[f][i][j] = (sigmoid(input_dxdy0[f][i][j]) + xy_grid0[f][i][j]) * stride;
                    input_dxdy1[f][i][j] = (sigmoid(input_dxdy1[f][i][j]) + xy_grid1[f][i][j]) * stride;
                    input_dxdy2[f][i][j] = (sigmoid(input_dxdy2[f][i][j]) + xy_grid2[f][i][j]) * stride;
                    // TODO: Uncomment this
                    input_dwdh0[f][i][j] = fm_t(exp(input_dwdh0[f][i][j])) * *(anchor + f) * stride;
                    input_dwdh1[f][i][j] = fm_t(exp(input_dwdh1[f][i][j])) * *(anchor + 2 + f) * stride;
                    input_dwdh2[f][i][j] = fm_t(exp(input_dwdh2[f][i][j])) * *(anchor + 4 + f) * stride;

                    //std::cout << "f = " << f << " and value = " << *(anchor+f) << std::endl;
                    // std::cout << "f = " << f << " and value = " <<  *(anchor + 2 + f) << ", input_dwdh1[f][i][j] = " << input_dwdh1[f][i][j] << std::endl;
                    // std::cout << "f = " << f << " and value = " <<  *(anchor + 4 + f) << std::endl;
                }
    // std::cout << *(anchor) << std::endl;
    // std::cout << *(anchor+1) << std::endl;
    // std::cout << *(anchor+2) << std::endl;
    // std::cout << *(anchor+3) << std::endl;
    // std::cout << *(anchor+4) << std::endl;
    // std::cout << *(anchor+5) << std::endl;
    // std::cout("*(*(anchor)+1) = %f\n",*(*(anchor)+1));
    // std::cout("*(*(anchor+1)) = %f\n",*(*(anchor+1)));
    // std::cout("*(*(anchor+1)+1) = %f\n",*(*(anchor+1)+1));
    // std::cout("*(*(anchor+2)) = %f\n",*(*(anchor+2)));
    // std::cout("*(*(anchor+2)+1) = %f\n",*(*(anchor+2)+1));
    //std::cout("*(*(anchor)) = %f\n");

    for (int f = 0; f < 1; f++)
            for(int i = 0; i < (input_h); i++)
                for(int j = 0; j < (input_w); j++)
                {
                    input_conf0[f][i][j] = sigmoid(input_conf0[f][i][j]);
                    input_conf1[f][i][j] = sigmoid(input_conf1[f][i][j]);
                    input_conf2[f][i][j] = sigmoid(input_conf2[f][i][j]);
                }
    
    for (int f = 0; f < 80; f++)
            for(int i = 0; i < (input_h); i++)
                for(int j = 0; j < (input_w); j++)
                {
                    input_prob0[f][i][j] = sigmoid(input_prob0[f][i][j]);
                    input_prob1[f][i][j] = sigmoid(input_prob1[f][i][j]);
                    input_prob2[f][i][j] = sigmoid(input_prob2[f][i][j]);
                }

    for (int f = 0; f < 85; f++)
        for(int i = 0; i < input_h; i++)
            for(int j = 0; j < input_w; j++)
            {
                if ((f==0) || (f==1))
                {
                    out_buf[(f*input_h*input_w)+(i*input_w)+(j)] = input_dxdy0[f][i][j];
                    out_buf[((f+85)*input_h*input_w)+(i*input_w)+(j)] = input_dxdy1[f][i][j];
                    out_buf[((f+170)*input_h*input_w)+(i*input_w)+(j)] = input_dxdy2[f][i][j];
                }
                else if((f==2) || (f==3))
                {
                    out_buf[(f*input_h*input_w)+(i*input_w)+(j)] = input_dwdh0[f-2][i][j];
                    out_buf[((f+85)*input_h*input_w)+(i*input_w)+(j)] = input_dwdh1[f-2][i][j];
                    out_buf[((f+170)*input_h*input_w)+(i*input_w)+(j)] = input_dwdh2[f-2][i][j];
                }
                else if(f==4)
                {
                    out_buf[(f*input_h*input_w)+(i*input_w)+(j)] = input_conf0[f-4][i][j];
                    out_buf[((f+85)*input_h*input_w)+(i*input_w)+(j)] = input_conf1[f-4][i][j];
                    out_buf[((f+170)*input_h*input_w)+(i*input_w)+(j)] = input_conf2[f-4][i][j];
                }
                else
                {
                    out_buf[(f*input_h*input_w)+(i*input_w)+(j)] = input_prob0[f-5][i][j];
                    out_buf[((f+85)*input_h*input_w)+(i*input_w)+(j)] = input_prob1[f-5][i][j];
                    out_buf[((f+170)*input_h*input_w)+(i*input_w)+(j)] = input_prob2[f-5][i][j];
                }
            }  
}
