#include "dcl.h"
#include <iostream>
#include <fstream>
#include <cmath>

using namespace std;

void load_tensor(const char* filename, wide_t tensor[][N][dk / 32], int D) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    size_t elements_read = fread(tensor, sizeof(wide_t), B * N * (D / 32), file);
    if (elements_read != B * N * (D / 32)) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        fclose(file);
        exit(1);
    }

    fclose(file);
}

int main() {
    wide_t Q[B][N][dk / 32];
    wide_t K[B][N][dk / 32];
    wide_t V[B][N][dv / 32];
    wide_t Output_ref[B][N][dv / 32];
    wide_t Output_HLS[B][N][dv / 32];

    load_tensor("Q_tensor.bin", Q, dk);
    load_tensor("K_tensor.bin", K, dk);
    load_tensor("V_tensor.bin", V, dv);
    load_tensor("Output_tensor.bin", Output_ref, dv);

    for (int i = 0; i < B; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < dv / 32; k++) {
                Output_HLS[i][j][k] = 0;
            }
        }
    }

    compute_attention_HLS(Q, K, V, Output_HLS);

    float error = 0;
    for (int i = 0; i < B; i++) {
        for (int j = 0; j < N; j++) {
            for (int k = 0; k < dv; k++) {
                int index_wide = k / 32;
                int index_sub = k % 32;

                fixed_t val_HLS = ((fixed_t*)&Output_HLS[i][j][index_wide])[index_sub];
                fixed_t val_ref = ((fixed_t*)&Output_ref[i][j][index_wide])[index_sub];

                error += std::pow(val_HLS.to_float() - val_ref.to_float(), 2);
            }
        }
    }

    error = error / (B * N * dv);
    printf("MSE: %.8f\n", error);

    return 0;
}