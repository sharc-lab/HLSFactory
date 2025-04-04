#include "dcl.h"

using namespace std;


void softmax(float matrix[B][N][N]) {
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            float max_val = matrix[b][i][0];
            for (int j = 1; j < N; ++j) {
                if (matrix[b][i][j] > max_val) {
                    max_val = matrix[b][i][j];
                }
            }

            float sum = 0;
            for (int j = 0; j < N; ++j) {
                matrix[b][i][j] = exp(matrix[b][i][j] - max_val);
                sum += matrix[b][i][j];
            }

            for (int j = 0; j < N; ++j) {
                matrix[b][i][j] /= sum;
            }
        }
    }
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
            }
        }
    }

    // Apply softmax
    softmax(attention);

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
