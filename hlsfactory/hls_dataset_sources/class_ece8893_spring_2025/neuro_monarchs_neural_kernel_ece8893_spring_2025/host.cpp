// host code

#include "dcl.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

// dimensions defined
#define BATCH_SIZE 1
#define IN_CH   256
#define IN_H    40
#define IN_W    40
#define OUT_CH  512
#define OUT_H   20
#define OUT_W   20
#define FC_IN   512
#define FC_OUT  512

// load file from binary
float**** loadBinary(const char* filename, int C1, int C2, int C3, int C4)
{
    float**** array = (float****)std::malloc(C1 * sizeof(float***));
    if (!array) { std::cerr << "[loadBinary] malloc C1 failed\n"; return nullptr; }

    for (int i = 0; i < C1; ++i) {
        array[i] = (float***)std::malloc(C2 * sizeof(float**));
        if (!array[i]) { std::cerr << "[loadBinary] malloc C2 failed\n"; return nullptr; }
        for (int j = 0; j < C2; ++j) {
            array[i][j] = (float**)std::malloc(C3 * sizeof(float*));
            if (!array[i][j]) { std::cerr << "[loadBinary] malloc C3 failed\n"; return nullptr; }
            for (int k = 0; k < C3; ++k) {
                array[i][j][k] = (float*)std::malloc(C4 * sizeof(float));
                if (!array[i][j][k]) { std::cerr << "[loadBinary] malloc C4 failed\n"; return nullptr; }
            }
        }
    }

    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) { std::cerr << "[loadBinary] cannot open " << filename << "\n"; return nullptr; }

    for (int i = 0; i < C1; ++i)
        for (int j = 0; j < C2; ++j)
            for (int k = 0; k < C3; ++k) {
                ifs.read(reinterpret_cast<char*>(array[i][j][k]), C4 * sizeof(float));
                if (!ifs) { std::cerr << "[loadBinary] read error\n"; return nullptr; }
            }
    ifs.close();
    return array;
}

// kernel declaration
extern "C" void last_layer_kernel(
    const float  input_data[IN_CH][IN_H][IN_W],
    float        output    [FC_OUT],
    const float  block1_conv1_weights  [OUT_CH][IN_CH][3][3],
    const float  block1_conv1_bn_gamma[OUT_CH],
    const float  block1_conv1_bn_beta [OUT_CH],
    const float  block1_conv1_bn_mean [OUT_CH],
    const float  block1_conv1_bn_var  [OUT_CH],

    const float  block1_conv2_weights  [OUT_CH][OUT_CH][3][3],
    const float  block1_conv2_bn_gamma[OUT_CH],
    const float  block1_conv2_bn_beta [OUT_CH],
    const float  block1_conv2_bn_mean [OUT_CH],
    const float  block1_conv2_bn_var  [OUT_CH],

    const float  block1_down_w        [OUT_CH][IN_CH][1][1],
    const float  block1_down_bn_gamma[OUT_CH],
    const float  block1_down_bn_beta [OUT_CH],
    const float  block1_down_bn_mean [OUT_CH],
    const float  block1_down_bn_var  [OUT_CH],

    const float  block2_conv1_weights  [OUT_CH][OUT_CH][3][3],
    const float  block2_conv1_bn_gamma[OUT_CH],
    const float  block2_conv1_bn_beta [OUT_CH],
    const float  block2_conv1_bn_mean [OUT_CH],
    const float  block2_conv1_bn_var  [OUT_CH],

    const float  block2_conv2_weights  [OUT_CH][OUT_CH][3][3],
    const float  block2_conv2_bn_gamma[OUT_CH],
    const float  block2_conv2_bn_beta [OUT_CH],
    const float  block2_conv2_bn_mean [OUT_CH],
    const float  block2_conv2_bn_var  [OUT_CH],

    const float  fc_weights[FC_OUT][FC_IN],
    const float  fc_bias   [FC_OUT]
);

// weight buffer definition
static float block1_conv1_weights  [OUT_CH][IN_CH][3][3];
static float block1_conv1_bn_gamma[OUT_CH];
static float block1_conv1_bn_beta [OUT_CH];
static float block1_conv1_bn_mean [OUT_CH];
static float block1_conv1_bn_var  [OUT_CH];

static float block1_conv2_weights  [OUT_CH][OUT_CH][3][3];
static float block1_conv2_bn_gamma[OUT_CH];
static float block1_conv2_bn_beta [OUT_CH];
static float block1_conv2_bn_mean [OUT_CH];
static float block1_conv2_bn_var  [OUT_CH];

static float block1_down_w        [OUT_CH][IN_CH][1][1];
static float block1_down_bn_gamma[OUT_CH];
static float block1_down_bn_beta [OUT_CH];
static float block1_down_bn_mean [OUT_CH];
static float block1_down_bn_var  [OUT_CH];

static float block2_conv1_weights  [OUT_CH][OUT_CH][3][3];
static float block2_conv1_bn_gamma[OUT_CH];
static float block2_conv1_bn_beta [OUT_CH];
static float block2_conv1_bn_mean [OUT_CH];
static float block2_conv1_bn_var  [OUT_CH];

static float block2_conv2_weights  [OUT_CH][OUT_CH][3][3];
static float block2_conv2_bn_gamma[OUT_CH];
static float block2_conv2_bn_beta [OUT_CH];
static float block2_conv2_bn_mean [OUT_CH];
static float block2_conv2_bn_var  [OUT_CH];

static float fc_weights[FC_OUT][FC_IN];
static float fc_bias   [FC_OUT];

// mse calculator
static float mse_512(const float *a, const float *b)
{
    double acc = 0.0;
    for (int i = 0; i < FC_OUT; ++i) {
        const double d = static_cast<double>(a[i]) - static_cast<double>(b[i]);
        acc += d * d;
    }
    return static_cast<float>(acc / FC_OUT);
}


int main()
{
    std::cout << "[main] Starting program\n";

    // load weights locally
    std::ifstream wfs("last_layer_weights_clean.bin", std::ios::binary);
    if (!wfs) { std::cerr << "[main] ERROR: cannot open weight file\n"; return EXIT_FAILURE; }

#define READ_BUF(buf) wfs.read(reinterpret_cast<char*>(buf), sizeof(buf))
    READ_BUF(block1_conv1_weights);  READ_BUF(block1_conv1_bn_gamma);
    READ_BUF(block1_conv1_bn_beta);  READ_BUF(block1_conv1_bn_mean);
    READ_BUF(block1_conv1_bn_var);   READ_BUF(block1_conv2_weights);
    READ_BUF(block1_conv2_bn_gamma); READ_BUF(block1_conv2_bn_beta);
    READ_BUF(block1_conv2_bn_mean);  READ_BUF(block1_conv2_bn_var);
    READ_BUF(block1_down_w);         READ_BUF(block1_down_bn_gamma);
    READ_BUF(block1_down_bn_beta);   READ_BUF(block1_down_bn_mean);
    READ_BUF(block1_down_bn_var);    READ_BUF(block2_conv1_weights);
    READ_BUF(block2_conv1_bn_gamma); READ_BUF(block2_conv1_bn_beta);
    READ_BUF(block2_conv1_bn_mean);  READ_BUF(block2_conv1_bn_var);
    READ_BUF(block2_conv2_weights);  READ_BUF(block2_conv2_bn_gamma);
    READ_BUF(block2_conv2_bn_beta);  READ_BUF(block2_conv2_bn_mean);
    READ_BUF(block2_conv2_bn_var);   READ_BUF(fc_weights);
    READ_BUF(fc_bias);
#undef READ_BUF
    wfs.close();
    std::cout << "[main] Weights loaded\n";

    // load first input sample only
    float**** input4d = loadBinary("input_1.bin", BATCH_SIZE, IN_CH, IN_H, IN_W);
    if (!input4d) { std::cerr << "[main] ERROR: loadBinary failed\n"; return EXIT_FAILURE; }

    float sample_in[IN_CH][IN_H][IN_W];
    for (int c = 0; c < IN_CH; ++c)
        for (int h = 0; h < IN_H; ++h)
            for (int w = 0; w < IN_W; ++w)
                sample_in[c][h][w] = input4d[0][c][h][w];

    std::cout << "[main] First input sample ready\n";

    // load expected outputs
    float golden[FC_OUT];
    std::ifstream gfs("output_1.bin", std::ios::binary);
    if (!gfs) { std::cerr << "[main] ERROR: cannot open output_1.bin\n"; return EXIT_FAILURE; }
    gfs.read(reinterpret_cast<char*>(golden), sizeof(golden));  // only first 512 floats
    if (!gfs) { std::cerr << "[main] ERROR: reading golden outputs failed\n"; return EXIT_FAILURE; }
    gfs.close();
    std::cout << "[main] Golden output loaded\n";

    // call fpga to run code
    float sample_out[FC_OUT] = {0};

    last_layer_kernel(
        sample_in, sample_out,
        block1_conv1_weights,  block1_conv1_bn_gamma,  block1_conv1_bn_beta,
        block1_conv1_bn_mean,  block1_conv1_bn_var,
        block1_conv2_weights,  block1_conv2_bn_gamma,  block1_conv2_bn_beta,
        block1_conv2_bn_mean,  block1_conv2_bn_var,
        block1_down_w,         block1_down_bn_gamma,   block1_down_bn_beta,
        block1_down_bn_mean,   block1_down_bn_var,
        block2_conv1_weights,  block2_conv1_bn_gamma,  block2_conv1_bn_beta,
        block2_conv1_bn_mean,  block2_conv1_bn_var,
        block2_conv2_weights,  block2_conv2_bn_gamma,  block2_conv2_bn_beta,
        block2_conv2_bn_mean,  block2_conv2_bn_var,
        fc_weights,            fc_bias
    );

    // compute MSE output
    float mse = mse_512(sample_out, golden);
    std::cout << "MSE for first sample: " << mse << "\n";

    std::cout << "Predicted (first 10):";
    for (int i = 0; i < 10; ++i) std::cout << " " << sample_out[i];
    std::cout << "\nGolden    (first 10):";
    for (int i = 0; i < 10; ++i) std::cout << " " << golden[i];
    std::cout << "\n";

    // free memory
    for (int i = 0; i < BATCH_SIZE; ++i) {
        for (int j = 0; j < IN_CH; ++j) {
            for (int k = 0; k < IN_H; ++k) std::free(input4d[i][j][k]);
            std::free(input4d[i][j]);
        }
        std::free(input4d[i]);
    }
    std::free(input4d);

    std::cout << "[main] Done – exiting\n";
    return EXIT_SUCCESS;
}
