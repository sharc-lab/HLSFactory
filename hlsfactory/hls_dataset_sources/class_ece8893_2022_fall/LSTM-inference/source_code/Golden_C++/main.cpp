#include <iostream>
#include "LSTM_Unit.h"

const int NUM_UNITS = 5;
const int SEQ_LENGTH = 59;

double lstmInference(const double inputs[SEQ_LENGTH], const double w_array[NUM_UNITS*4], const double w_bias[NUM_UNITS*4],
                     double u_array[NUM_UNITS][NUM_UNITS*4], const double u_bias[NUM_UNITS * 4],
                     const double dense_weights[NUM_UNITS], double dense_bias);

int main() {
    // input sequence
    double inputs[SEQ_LENGTH]{-0.0307, -0.0095, -0.0028,  0.0184,  0.0532,  0.0364,  0.0610,  0.0404,
                              0.0213,  0.0312,  0.0324,  0.1066,  0.1260,  0.1177,  0.2406,  0.2645,
                              0.1950,  0.2056,  0.2196,  0.2314,  0.2446,  0.2987,  0.2964,  0.2810,
                              0.2810,  0.3070,  0.3387,  0.3162,  0.2862,  0.2319,  0.2361,  0.2323,
                              0.2437,  0.3448,  0.3576,  0.3841,  0.3609,  0.4242,  0.4294,  0.4181,
                              0.3890,  0.3879,  0.4089,  0.4025,  0.3964,  0.3886,  0.3886,  0.3983,
                              0.3765,  0.3860,  0.3720,  0.3720,  0.3994,  0.4465,  0.4339,  0.4526,
                              0.4098,  0.3614,  0.4122};

    // array of input weights
    double w_array[NUM_UNITS * 4]{-0.7036, -0.3266,  0.1495, -0.2934,  0.3067, -0.7659,  0.1118, -0.2363,
                                  -0.1715,  0.4593,  0.1756,  0.5064, -0.5176, -0.7126,  0.0753, -0.7631,
                                  -0.0373, -0.2063, -0.5267,  0.0079};

    // bias array
    double w_bias[NUM_UNITS * 4]{0.1774,  0.6707,  0.0083,  0.0358, -0.1736, -0.4170, -0.3332,  0.1679,
                                 0.1298, -0.4440, -0.2245, -0.0826, -0.0784,  0.4829, -0.3087,  0.2438,
                                 0.2631,  0.1420,  0.5892, -0.2122};

    // array of hidden-state t-1 weights
    double u_array[NUM_UNITS][NUM_UNITS * 4] = {{-0.0916, -0.2958, -0.3029,  0.4635,
                                                 -0.3798, -0.6660,  0.3647, -0.3648,
                                                 -0.2473,  0.1977,  0.1243,  0.1642,
                                                 0.5087,  0.2351,  0.3023, -0.6224,
                                                 -0.4155,  0.0496,  0.1967,  0.4653},
                                                {0.2413, -0.3172, -0.3208, -0.0284,
                                                 0.5316, -0.3189, -0.2269, -0.0008,
                                                 0.1918, -0.1530,  0.3306, -0.1718,
                                                 -0.2074, -0.1121, -0.5950, -0.4636,
                                                 0.1989, -0.0902, -0.4454,  0.6904},
                                                {-0.2769, -0.3546,  0.4945,  0.2046,
                                                 0.0170,  0.4559,  0.4595,  0.1425,
                                                 0.3532, -0.1646, -0.6491, -0.2808,
                                                 -0.1576, -0.0327,  0.1833, -0.2814,
                                                 0.2739, -0.0235, -0.1867,  0.0802},
                                                {-0.0501,  0.1581, -0.1047,  0.2665,
                                                 -0.1189, -0.0026,  0.1278,  0.0753,
                                                 -0.0041, -0.1752,  0.0652,  0.1395,
                                                 0.0660,  0.5631,  0.1124,  0.5302,
                                                 0.0814,  0.5548,  0.1739, -0.4648},
                                                {-0.5395, -0.0421, -0.1753,  0.0589,
                                                 0.3069, -0.1285, -0.3267, -0.1625,
                                                 -0.1848,  0.0712,  0.0601,  0.3863,
                                                 -0.7180, -0.4330, -0.4514, -0.0106,
                                                 -0.0300, -0.6793, -0.6978,  0.1530}};

    // hh bias
    double u_bias[NUM_UNITS * 4]{-0.3236,  0.4668,  0.6738,  0.5599,
                                 0.1805, -0.3578, -0.0777, -0.0739,
                                 0.0458, -0.3726, -0.1154,  0.0794,
                                 0.1378,  0.0076,  0.0043, -0.2654,
                                 0.6521,  0.6662,  0.3005, -0.4639};

    // Dense Layer
    double dense_weights[NUM_UNITS] = {0.4693,  0.5267, -0.6509, -0.6532, -0.1366};
    double dense_bias = 0.4483;

    // Perform Inference
    double result = lstmInference(inputs, w_array, w_bias, u_array, u_bias, dense_weights, dense_bias);

    // Inverse Transform
    std::cout << "LSTM Inference: " << ((result+3.40463247)/0.02363507) <<"\n";

    return 0;
}

double lstmInference(const double inputs[SEQ_LENGTH], const double w_array[NUM_UNITS*4], const double w_bias[NUM_UNITS*4],
                     double u_array[NUM_UNITS][NUM_UNITS*4], const double u_bias[NUM_UNITS * 4],
                     const double dense_weights[NUM_UNITS], double dense_bias)
{
    // global variables
    double adjusted_inputs[NUM_UNITS * 4];
    LSTM_Unit units[NUM_UNITS]; // array of lstm units

    // LSTM layer
    for (int j = 0; j < SEQ_LENGTH; j++) {
        // Adjusting Inputs -- Matrix Calculation
        for (int i = 0; i < NUM_UNITS; i++)
        {
            // time sequence inputs
            adjusted_inputs[i * 4] = inputs[j] * w_array[i];
            adjusted_inputs[i * 4 + 1] = inputs[j] * w_array[NUM_UNITS * 1 + i];
            adjusted_inputs[i * 4 + 2] = inputs[j] * w_array[NUM_UNITS * 2 + i];
            adjusted_inputs[i * 4 + 3] = inputs[j] * w_array[NUM_UNITS * 3 + i];

            // hidden states pass on
            for (int k = 0; k < NUM_UNITS; k++)
            {
                adjusted_inputs[i * 4] += units[k].get_hidden_state_() * u_array[k][i];
                adjusted_inputs[i * 4 + 1] += units[k].get_hidden_state_() * u_array[k][NUM_UNITS * 1 + i];
                adjusted_inputs[i * 4 + 2] += units[k].get_hidden_state_() * u_array[k][NUM_UNITS * 2 + i];
                adjusted_inputs[i * 4 + 3] += units[k].get_hidden_state_() * u_array[k][NUM_UNITS * 3 + i];
            }

            // add bias
            adjusted_inputs[i * 4] += (w_bias[i] + u_bias[i]);
            adjusted_inputs[i * 4 + 1] += (w_bias[NUM_UNITS * 1 + i] + u_bias[NUM_UNITS * 1 + i]);
            adjusted_inputs[i * 4 + 2] += (w_bias[NUM_UNITS * 2 + i] + u_bias[NUM_UNITS * 2 + i]);
            adjusted_inputs[i * 4 + 3] += (w_bias[NUM_UNITS * 3 + i] + u_bias[NUM_UNITS * 3 + i]);
        }

        // Unit Operations
        for (int i = 0; i < NUM_UNITS; i++)
        {
            units[i].performInference(adjusted_inputs[i * 4], adjusted_inputs[i * 4 + 1], adjusted_inputs[i * 4 + 2], adjusted_inputs[i * 4 + 3]);
        }
    }
    double dense_result = 0;
    for (int i = 0; i < NUM_UNITS; i++)
    {
        dense_result += dense_weights[i] * units[i].get_hidden_state_();
    }
    dense_result += dense_bias;

    return dense_result;
}