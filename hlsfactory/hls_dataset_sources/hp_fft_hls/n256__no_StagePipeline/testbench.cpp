
#include "FFT.h"


void fft_sw_gold(complex<dtype_gold>* input, complex<dtype_gold>* output) {
    // const double PI = acos(-1);  // Define pi
    for (int k = 0; k < FFT_NUM; ++k) {
        complex<dtype_gold> sum = 0;  // Initialize the sum for G[k]
        for (int n = 0; n < FFT_NUM; ++n) {
            // Calculate the exponential term
            double angle = -2.0 * PI * k * n / FFT_NUM;
            std::complex<dtype_gold> exp_term(cos(angle), sin(angle));
            sum += input[n] * exp_term;  // Accumulate the sum
        }
        output[k] = sum;  // Store the result
    }
}


int main()
{
    complex<dtype_test> data[batch_size][FFT_NUM], dataFq[batch_size][FFT_NUM];
    complex<dtype_gold> data_gold[batch_size][FFT_NUM], dataFq_gold[batch_size][FFT_NUM];
    

    for (int b = 0; b < batch_size; b++){
        for (int i = 0; i < FFT_NUM; i++) {
            float t = static_cast<float>(i) / FFT_NUM;  // Normalized time variable
            float real_part = std::sin(2.0 * M_PI * 10.0 * t) + 0.5 * std::cos(2.0 * M_PI * 50.0 * t);
            float imag_part = std::exp(-5.0 * t) * std::sin(2.0 * M_PI * 20.0 * t);
            data[b][i] = complex<float>(real_part, imag_part);
            data_gold[b][i] = complex<float>(real_part, imag_part);
        }
    }


    cout << "FFT run FFT_TOP" << endl;
    hls::stream<hls::vector<complex<float>, UF*2>>  xn_input_strm;
    hls::stream<hls::vector<complex<float>, UF*2>>  xk_output_strm;
    for (int b = 0; b < 2; b++){
        FFT_TOP(data[b], dataFq[b]);
    }
    
    for (int b = 0; b < batch_size; b++){
        fft_sw_gold(data_gold[b], dataFq_gold[b]);
    }

    cout << "FFT Comparison" << endl;
    for (int i = 0; i < FFT_NUM; i++)
        cout << dataFq[0][i] << dataFq_gold[0][i] << endl;
    
    cout << endl << "FFT difference" << endl;
    cout << fixed << setprecision(4);  // to 4
    for (int i = 0; i < FFT_NUM; i++)
        cout << complex<dtype_gold>(dataFq[0][i]) - dataFq_gold[0][i] << endl;

    return 0;
}
