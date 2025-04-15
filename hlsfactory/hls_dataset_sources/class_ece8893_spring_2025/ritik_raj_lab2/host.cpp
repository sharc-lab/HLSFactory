#include "dcl.h"

using namespace std;

void load_tensor(const char* filename, fixed_t tensor[][N][dk], int D) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    size_t elements_read = fread(tensor, sizeof(fixed_t), B * N * D, file);
    if (elements_read != B * N * D) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        fclose(file);
        exit(1);
    }

    fclose(file);
}


int main() {
    // Allocate memory for fixed_t tensors
    fixed_t Q[B][N][dk];
    fixed_t K[B][N][dk];
    fixed_t V[B][N][dv];
    fixed_t Output_ref[B][N][dv];
	fixed_t Output_HLS[B][N][dv];

    // Allocate memory for wide_t tensors
    wide_t Q_wide[B][N][dk/64];
    wide_t K_wide[B][N][dk/64];
    wide_t V_wide[B][N][dv/64];
    wide_t Output_HLS_wide[B][N][dv/64];

    // Load tensors from binary files
    load_tensor("Q_tensor.bin", Q, dk);
    load_tensor("K_tensor.bin", K, dk);
    load_tensor("V_tensor.bin", V, dv);
	load_tensor("Output_tensor.bin", Output_ref, dv);

	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv; k++) {
				Output_HLS[i][j][k] = 0;
			}
		}
	}

    // Packing fixed_t Q, K, V into wide_t Q,K,V
    for (int b = 0; b < B; b++) {
        for (int n = 0; n < N; n++) {
            for (int k = 0; k < dk/64; k++) {

                wide_t packed_Q = 0, packed_K = 0, packed_V = 0, packed_ref = 0;

                for (int i = 0; i < 64; i++) {
                    packed_Q.range((i + 1) * 16 - 1, i * 16) = Q[b][n][k*64+i];
                    packed_K.range((i + 1) * 16 - 1, i * 16) = K[b][n][k*64+i];
                    packed_V.range((i + 1) * 16 - 1, i * 16) = V[b][n][k*64+i];
                }
                Q_wide[b][n][k] = packed_Q;
                K_wide[b][n][k] = packed_K;
                V_wide[b][n][k] = packed_V;
            }
        }
    }

    // call HLS kernel
    compute_attention_HLS(Q_wide, K_wide, V_wide, Output_HLS_wide);
	
    // Unpacking wide_t Output_HLS into fixed_t Output_HLS
    for (int b = 0; b < B; b++) {
        for (int n = 0; n < N; n++) {
            for (int k = 0; k < dk/64; k++)  {
                for (int i = 0; i < 64; i++){
                    Output_HLS[b][n][i+k*64] = Output_HLS_wide[b][n][k].range((i + 1) * 16 - 1, i * 16);
                }
            }
        }
    }


	float error = 0;
	// compare HLS output and reference output tensor
	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv; k++) {
				error += std::pow(Output_HLS[i][j][k].to_float() - Output_ref[i][j][k].to_float(), 2);
                //printf("Output_HLS[%d][%d][%d]: %.8f; Output_ref[%d][%d][%d]: %.8f\n", 
                //i, j, k, Output_HLS[i][j][k].to_float(), i, j, k, Output_ref[i][j][k].to_float());
			}
		}
	}
	error = error / (B * N * dv);
	printf("MSE: %.8f\n", error);

    return 0;
}
