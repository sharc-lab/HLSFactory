#include <cmath>
#include <cstdio>
#include <cstdlib>

// Add these definitions at the top (must match top.cpp)
typedef float fixed_t;  // Using float as in top.cpp
const int BATCH = 16;
const int K = 4;
const int L = 256;

// Forward declaration of the function from top.cpp
extern "C" void circular_convolution_3d(fixed_t A[BATCH][K][L],
                                       fixed_t B[BATCH][K][L],
                                       fixed_t C[BATCH][K][L]);

void load_tensor(const char* filename, fixed_t tensor[BATCH][K][L]) {
    FILE* file = fopen(filename, "rb");
    if (!file) { 
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    size_t elements_read = fread(tensor, sizeof(fixed_t), BATCH * K * L, file);
    if (elements_read != BATCH * K * L) {
        fprintf(stderr, "Error reading file: %s (got %zu elements)\n", filename, elements_read);
        fclose(file);
        exit(1);
    }

    fclose(file);
}

int main() {
    fixed_t A[BATCH][K][L];
    fixed_t B[BATCH][K][L];
    fixed_t Output_ref[BATCH][K][L];
    fixed_t Output_HLS[BATCH][K][L];

    // Load binary tensors
    load_tensor("input1_A.bin", A);
    load_tensor("input1_B.bin", B);
    load_tensor("output1_C.bin", Output_ref);

    // Initialize output
    for (int i = 0; i < BATCH; i++) {
        for (int j = 0; j < K; j++) {
            for (int k = 0; k < L; k++) {
                Output_HLS[i][j][k] = 0;
            }
        }
    }

    // Call HLS kernel
    circular_convolution_3d(A, B, Output_HLS);

    // Compute MSE
    float mse = 0.0f;
    for (int i = 0; i < BATCH; i++) {
        for (int j = 0; j < K; j++) {
            for (int k = 0; k < L; k++) {
                float diff = Output_HLS[i][j][k] - Output_ref[i][j][k];
                mse += diff * diff;
            }
        }
    }

    mse /= (BATCH * K * L);
    printf("MSE: %.8f\n", mse);

    return 0;
}