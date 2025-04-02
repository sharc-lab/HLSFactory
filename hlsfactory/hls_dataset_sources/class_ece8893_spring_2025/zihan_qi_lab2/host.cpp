
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
    // Allocate memory for tensors
    fixed_t Q[B][N][dk];
    fixed_t K[B][N][dk];
    fixed_t V[B][N][dv];
    fixed_t Output_ref[B][N][dv];
	fixed_t Output_HLS[B][N][dv];

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

    static vec32_t Q_vec32[B][N][dk / 32];  
    static vec32_t K_vec32[B][N][dk / 32];  
    static vec32_t V_vec32[B][N][dv / 32];
    static vec32_t Output_HLS_vec32[B][N][dv / 32];

    // Change: convert fix16 to vec32 for Q,K,V
    for (int b = 0; b < B; b++) {
        for (int n = 0; n < N; n++) {
            for (int i = 0; i < dk / 32; i++) {
                vec32_t temp_Q, temp_K;
                for (int j = 0; j < 32; j++) {
                    temp_Q[j] = Q[b][n][i * 32 + j];
                    temp_K[j] = K[b][n][i * 32 + j];
                }
                Q_vec32[b][n][i] = temp_Q;
                K_vec32[b][n][i] = temp_K;
            }

            for (int i = 0; i < dv / 32; i++) {
                vec32_t temp_V;
                for (int j = 0; j < 32; j++) {
                    temp_V[j] = V[b][n][i * 32 + j];
                }
                V_vec32[b][n][i] = temp_V;
            }
        }
    }
    // call HLS kernel
    compute_attention_HLS(Q_vec32, K_vec32, V_vec32, Output_HLS_vec32);
    //compute_attention_HLS(Q_vec32, K_vec32, V_vec32, Output_HLS);
    //compute_attention_HLS(Q_vec32, K_vec32, V, Output_HLS);
    //compute_attention_HLS(Q, K, V, Output_HLS);

    // // Change: convert vec32 to fix16 for Output_HLS
    for (int b = 0; b < B; b++) {
        for (int n = 0; n < N; n++) {
            for (int i = 0; i < dv / 32; i++) {
                vec32_t temp_Output = Output_HLS_vec32[b][n][i];
                for (int j = 0; j < 32; j++) {
                    Output_HLS[b][n][i * 32 + j] = temp_Output[j];
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
                printf("Output_HLS[%d][%d][%d]: %.8f; Output_ref[%d][%d][%d]: %.8f\n", i, j, k, Output_HLS[i][j][k].to_float(), i, j, k, Output_ref[i][j][k].to_float());
			}
		}
	}
	error = error / (B * N * dv);
	printf("MSE: %.8f\n", error);

    return 0;
}
