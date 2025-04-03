#include "dcl.h"

using namespace std;

// All the change there is just to see golden data distribution
void softmax(float matrix[B][N][N]) {
    // float temp_min_diff= 0.0;
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            float max_val = matrix[b][i][0];
            // // store min_val
            // float min_val = matrix[b][i][0];            
            for (int j = 1; j < N; ++j) {
                if (matrix[b][i][j] > max_val) {
                    max_val = matrix[b][i][j];
                }
                // // store min_val
                // if (matrix[b][i][j] <min_val) {
                //     min_val = matrix[b][i][j];
                // }
            }
            
            float sum = 0;
            for (int j = 0; j < N; ++j) {
                matrix[b][i][j] = exp(matrix[b][i][j] - max_val);
                sum += matrix[b][i][j];
            }

            for (int j = 0; j < N; ++j) {
                matrix[b][i][j] /= sum;
            }

            // print max_val and min_valJUsti to see the range.(In the lab parameter, difference is bigger than 2, so that the approxiamtion strategy may induce large MSE)
            // printf("b: %d, i: %d, max_val: %.8f; min_val:  %.8f; diff:  %.8f\n", b, i, max_val, min_val, min_val-max_val);
            // if((min_val-max_val) < temp_min_diff) temp_min_diff=min_val-max_val;
        }
    }
    // printf("Minimum value of the x in e^x is:%.8f\n", temp_min_diff);
}

void compute_attention(fixed_t Q[B][N][dk], fixed_t K[B][N][dk], fixed_t V[B][N][dv], fixed_t Output[B][N][dv]) {
    float attention[B][N][N];
    float scale = 1.0 / sqrt((float)dk);

    // Compute Q * K^T
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < N; ++j) {
                float sum = 0;
                for (int k = 0; k < dk; ++k) {
                    sum += Q[b][i][k].to_float() * K[b][j][k].to_float();
                }
                attention[b][i][j] = sum * scale;
                //printf("Golden: attention[%d][%d][%d]: %.8f; \n", b, i, j, attention[b][i][j]);
            }
        }
    }

    // Apply softmax
    softmax(attention);
    // Just to see the golden data distribution
    // printf("Post-attention");
    // for (int i = 0; i < B; i++)
    // {
    //     for(int j = 0; j < N; j++) {
	// 		for(int k = 0; k < N; k++) {
    //             printf("attention[%d][%d][%d]: %.8f; \n", i, j, k, attention[i][j][k]);
	// 		}
	// 	}
    // }

    // Compute Attention * V
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < dv; ++j) {
                float sum = 0;
                for (int k = 0; k < N; ++k) {
                    sum += attention[b][i][k] * V[b][k][j].to_float();
                }
                Output[b][i][j] = (fixed_t)sum;
            }
        }
    }
}

void load_tensor(const char* filename, fixed_t tensor[B][N][dk], int D) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file: " << filename << endl;
        exit(1);
    }
    file.read(reinterpret_cast<char*>(tensor), B * N * D * sizeof(fixed_t));
    file.close();
}

void save_tensor(const char* filename, fixed_t tensor[B][N][dv], int D) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Error opening file for writing: " << filename << endl;
        exit(1);
    }
    file.write(reinterpret_cast<char*>(tensor), B * N * D * sizeof(fixed_t));
    file.close();
}

int main() {
    // Allocate memory for tensors
    fixed_t Q[B][N][dk];
    fixed_t K[B][N][dk];
    fixed_t V[B][N][dv];
    fixed_t Output[B][N][dv];

    // Load tensors from binary files
    load_tensor("Q_tensor.bin", Q, dk);
    load_tensor("K_tensor.bin", K, dk);
    load_tensor("V_tensor.bin", V, dv);

    // Compute attention
    // Note: all intermediate computation in reference uses floating point
    compute_attention(Q, K, V, Output);


    // Save the output tensor to a binary file
    save_tensor("Output_tensor.bin", Output, dv);

    cout << "Attention computation completed and result saved to Output_tensor.bin" << endl;

    return 0;
}
