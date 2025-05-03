#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include "top.h"

#define TOLERANCE 0.005

template <int B, int C, int H, int W>
void load_4d_input(const char *filename, data_t tensor[B][C][H][W])
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    file.read(reinterpret_cast<char *>(tensor), sizeof(data_t) * B * C * H * W);
    file.close();
}


// Load 1D weights for batch normalization
template <int N> 
void load_1d_weights(const char *filename, data_t tensor[N])
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    file.read(reinterpret_cast<char *>(tensor), sizeof(data_t) * N);
    file.close();
}

// Load 2D weights for fully connected layer
template <int IN, int OUT> 
void load_2d_weights(const char *filename, data_t tensor[IN][OUT])
{
    std::ifstream file(filename, std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open: " << filename << std::endl;
        exit(EXIT_FAILURE);
    }
    file.read(reinterpret_cast<char *>(tensor), sizeof(data_t) * IN * OUT);
    file.close();
}

double compute_mse(data_t a[BATCH][OUT_CHANNELS],
                   data_t b[BATCH][OUT_CHANNELS])
{
    double mse = 0.0;
    int total = BATCH * OUT_CHANNELS;
    for (int bidx = 0; bidx < BATCH; ++bidx)
    {
        for (int oc = 0; oc < OUT_CHANNELS; ++oc)
        {
            double diff = a[bidx][oc] - b[bidx][oc];
            mse += diff * diff;
        }
    }
    return mse / total;
}

double compute_mse2(data_t a[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2],
                   data_t b[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2])
{
    double mse = 0.0;
    int total = BATCH * OUT_CHANNELS * (HEIGHT / 2) * (WIDTH / 2);
    for (int bidx = 0; bidx < BATCH; ++bidx)
    {
        for (int oc = 0; oc < OUT_CHANNELS; ++oc)
        {
            for (int h = 0; h < HEIGHT / 2; ++h)
            {
                for (int w = 0; w < WIDTH / 2; ++w)
                {
                    double diff = a[bidx][oc][h][w] - b[bidx][oc][h][w];
                    mse += diff * diff;
                }
            }
        }
    }
    return mse / total;
}

//Pack the input into a vector of size WIDTH
template <int B>
void pack_input(data_t input[B], hls::vector<data_t, B> input_vector)
{
    
                for (int w = 0; w < B; ++w)
                {
                    input_vector[w] = input[w];
                }
            
}

int main()
{
    // Static allocations
    static data_t input[BATCH][IN_CHANNELS][HEIGHT][WIDTH];
    static data_t block0_weights_conv1[OUT_CHANNELS][IN_CHANNELS][KERNEL_HEIGHT][KERNEL_WIDTH];
    static data_t block0_gamma1[OUT_CHANNELS];
    static data_t block0_beta1[OUT_CHANNELS];
    static data_t block0_mean1[OUT_CHANNELS];
    static data_t block0_var1[OUT_CHANNELS];
    static data_t block0_output_bn1[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    static data_t block0_weights_conv2[OUT_CHANNELS][OUT_CHANNELS][KERNEL_HEIGHT][KERNEL_WIDTH];
    static data_t block0_gamma2[OUT_CHANNELS];
    static data_t block0_beta2[OUT_CHANNELS];
    static data_t block0_mean2[OUT_CHANNELS];
    static data_t block0_var2[OUT_CHANNELS];
    static data_t block0_output_bn2[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    hls::vector<data_t,WIDTH/2> block0_output_bn2_vector [BATCH][OUT_CHANNELS][HEIGHT / 2];
    static data_t block0_weights_ds_conv1[OUT_CHANNELS][IN_CHANNELS][DS_KERNEL_HEIGHT][DS_KERNEL_WIDTH];
    static data_t block0_ds_gamma1[OUT_CHANNELS];
    static data_t block0_ds_beta1[OUT_CHANNELS];
    static data_t block0_ds_mean1[OUT_CHANNELS];
    static data_t block0_ds_var1[OUT_CHANNELS];
    static data_t output_block0[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    static data_t block1_weights_conv1[OUT_CHANNELS][OUT_CHANNELS][KERNEL_HEIGHT][KERNEL_WIDTH];
    static data_t block1_gamma1[OUT_CHANNELS];
    static data_t block1_beta1[OUT_CHANNELS];
    static data_t block1_mean1[OUT_CHANNELS];
    static data_t block1_var1[OUT_CHANNELS];
    static data_t block1_output_bn1[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    static data_t block1_weights_conv2[OUT_CHANNELS][OUT_CHANNELS][KERNEL_HEIGHT][KERNEL_WIDTH];
    static data_t block1_gamma2[OUT_CHANNELS];
    static data_t block1_beta2[OUT_CHANNELS];
    static data_t block1_mean2[OUT_CHANNELS];
    static data_t block1_var2[OUT_CHANNELS];
    static data_t block1_output_bn2[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    static data_t fc_weights[OUT_CHANNELS][OUT_CHANNELS];
    static data_t fc_bias[OUT_CHANNELS];
    static data_t fc_output[BATCH][OUT_CHANNELS];
    static data_t tanh_output[BATCH][OUT_CHANNELS];
    static data_t ref_output[BATCH][OUT_CHANNELS];
    static data_t output_local[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    hls::vector<data_t,WIDTH/2> output_local_vector [BATCH][OUT_CHANNELS][HEIGHT / 2];
    std::cout << "Memory allocated successfully!" << std::endl;
    static data_t block0_bn1_output_reference[BATCH][OUT_CHANNELS][HEIGHT / 2][WIDTH / 2];
    // Loading files
    load_4d_input<BATCH, IN_CHANNELS, HEIGHT, WIDTH>("input_1.bin", input);
    std::cout << "Input 1 loaded!" << std::endl;

    load_4d_input<OUT_CHANNELS, IN_CHANNELS, KERNEL_HEIGHT, KERNEL_WIDTH>("layer4_block0_conv1_weights.bin", block0_weights_conv1);
    std::cout << "Input 3 loaded!" << std::endl;

    // weights for batch norm 1 block0
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_1_gamma.bin", block0_gamma1);
    std::cout << "Input 4 loaded!" << std::endl;
    //pack_input<OUT_CHANNELS>(block0_gamma1, block0_gamma1_vector);
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_1_beta.bin", block0_beta1);
    std::cout << "Input 5 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_1_mean.bin", block0_mean1);
    std::cout << "Input 6 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_1_var.bin", block0_var1);
    std::cout << "Input 7 loaded!" << std::endl;

    // weights for conv2 block0
    load_4d_input<OUT_CHANNELS, OUT_CHANNELS, KERNEL_HEIGHT, KERNEL_WIDTH>("layer4_block0_conv2_weights.bin", block0_weights_conv2);
    std::cout << "Input 8 loaded!" << std::endl;
    // weights for batch norm 2 block0
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_2_gamma.bin", block0_gamma2);
    std::cout << "Input 9 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_2_beta.bin", block0_beta2);
    std::cout << "Input 10 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_2_mean.bin", block0_mean2);
    std::cout << "Input 11 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_batch_norm_2_var.bin", block0_var2);
    std::cout << "Input 12 loaded!" << std::endl;
    // weights for downsample conv block0
    load_4d_input<OUT_CHANNELS, IN_CHANNELS, DS_KERNEL_HEIGHT, DS_KERNEL_WIDTH>("layer4_block0_downsample_weights.bin", block0_weights_ds_conv1);
    std::cout << "Input 13 loaded!" << std::endl;
    // weights for downsample batch norm block0
    load_1d_weights<OUT_CHANNELS>("layer4_block0_downsample_batch_norm_gamma.bin", block0_ds_gamma1);
    std::cout << "Input 14 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_downsample_batch_norm_beta.bin", block0_ds_beta1);
    std::cout << "Input 15 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_downsample_batch_norm_mean.bin", block0_ds_mean1);
    std::cout << "Input 16 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block0_downsample_batch_norm_var.bin", block0_ds_var1);
    std::cout << "Input 17 loaded!" << std::endl;
    // weights for conv1 block1
    load_4d_input<OUT_CHANNELS, OUT_CHANNELS, KERNEL_HEIGHT, KERNEL_WIDTH>("layer4_block1_conv1_weights.bin", block1_weights_conv1);
    std::cout << "Input 18 loaded!" << std::endl;
    // weights for batch norm 1 block1
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_1_gamma.bin", block1_gamma1);
    std::cout << "Input 19 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_1_beta.bin", block1_beta1);
    std::cout << "Input 20 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_1_mean.bin", block1_mean1);
    std::cout << "Input 21 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_1_var.bin", block1_var1);
    std::cout << "Input 22 loaded!" << std::endl;
    // weights for conv2 block1
    load_4d_input<OUT_CHANNELS, OUT_CHANNELS, KERNEL_HEIGHT, KERNEL_WIDTH>("layer4_block1_conv2_weights.bin", block1_weights_conv2);
    std::cout << "Input 23 loaded!" << std::endl;
    // weights for batch norm 2 block1
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_2_gamma.bin", block1_gamma2);
    std::cout << "Input 24 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_2_beta.bin", block1_beta2);
    std::cout << "Input 25 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_2_mean.bin", block1_mean2);
    std::cout << "Input 26 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("layer4_block1_batch_norm_2_var.bin", block1_var2);
    std::cout << "Input 27 loaded!" << std::endl;
    // weights for fully connected layer
    load_2d_weights<OUT_CHANNELS, OUT_CHANNELS>("fc_weights.bin", fc_weights);
    std::cout << "Input 28 loaded!" << std::endl;
    load_1d_weights<OUT_CHANNELS>("fc_bias.bin", fc_bias);
    std::cout << "Input 29 loaded!" << std::endl;
    // load reference output
    load_2d_weights<BATCH, OUT_CHANNELS>("output_1.bin", ref_output);
    std::cout << "Input 30 loaded!" << std::endl;

    load_4d_input<BATCH, OUT_CHANNELS, HEIGHT / 2, WIDTH / 2>("block0_output.bin", block0_bn1_output_reference);

    // Call ResNet
    ResNet(input, block0_weights_conv1, 
           block0_gamma1, block0_beta1, block0_mean1, block0_var1, block0_output_bn1,
           block0_weights_conv2, block0_gamma2, block0_beta2, block0_mean2, block0_var2, block0_output_bn2_vector, output_local_vector,
           block0_weights_ds_conv1, block0_ds_gamma1, block0_ds_beta1, block0_ds_mean1, block0_ds_var1,
           output_block0, block1_weights_conv1, block1_gamma1, block1_beta1, block1_mean1, block1_var1,
           block1_output_bn1, block1_weights_conv2, block1_gamma2, block1_beta2, block1_mean2, block1_var2,
           block1_output_bn2, fc_weights, fc_bias, tanh_output);

    double mse = compute_mse(tanh_output, ref_output);
    
    std::cout << "MSE compared to the main reference: " << mse << std::endl;

    if (mse < TOLERANCE)
    {
        std::cout << "Test Passed." << std::endl;
        return 0;
    }
    else
    {
        std::cerr << "Test Failed! MSE = " << mse << std::endl;
        return 1;
    }
}
