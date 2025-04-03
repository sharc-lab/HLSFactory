#include "dcl.h"

using namespace std;

// Provided tensor loading function
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

// Loads tensor in the wide bus transaction data type
void load_tensor_large(const char* filename, mem_t tensor[][N][dk/32], int D) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    size_t elements_read = fread(tensor, sizeof(mem_t), B * N * D, file);
    if (elements_read != B * N * D) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        fclose(file);
        exit(1);
    }

    fclose(file);
}


int main() {
    // Allocate memory for tensors
    mem_t Q[B][N][dk/32];
    mem_t K[B][N][dk/32];
    mem_t V[B][N][dv/32];
	mem_t Output_HLS[B][N][dv/32];

    // Storage for reference and the converted output from the module
    fixed_t Output_HLS_convert[B][N][dv];
    fixed_t Output_ref[B][N][dv];

    // Load tensors from binary files
    load_tensor_large("Q_tensor.bin", Q, dk/32);
    load_tensor_large("K_tensor.bin", K, dk/32);
    load_tensor_large("V_tensor.bin", V, dv/32);
	load_tensor("Output_tensor.bin", Output_ref, dv);

    // Zeroing the module output matrix
	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv/32; k++) {
				Output_HLS[i][j][k] = 0;
			}
		}
	}

    // call HLS kernel
    compute_attention_HLS(Q, K, V, Output_HLS);

    // Converting the received wide bus values back into a standard fixed_t matrix
    mem_t data;
    for (int b = 0; b < B; b++) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < dv/32; k++) {
            #pragma HLS pipeline
                data = Output_HLS[b][i][k];
                for (int kk = 0; kk < 32; kk++) {
                    ap_uint<16> temp = data.range(15 + (kk * 16), 0 + (kk * 16));
                    Output_HLS_convert[b][i][k * 32 + kk].range(15, 0) = temp;
                }
            }
        }
    }
	
	float error = 0;
	// compare HLS output and reference output tensor
	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv; k++) {
                //printf("Reference: %f | Computed: %f\n", Output_ref[i][j][k].to_float(), Output_HLS_convert[i][j][k].to_float());
				error += std::pow(Output_HLS_convert[i][j][k].to_float() - Output_ref[i][j][k].to_float(), 2);
                //printf("Output_HLS[%d][%d][%d]: %.8f; Output_ref[%d][%d][%d]: %.8f\n", 
                //i, j, k, Output_HLS[i][j][k].to_float(), i, j, k, Output_ref[i][j][k].to_float());
			}
		}
	}
	error = error / (B * N * dv);
	printf("MSE: %.8f\n", error);

    return 0;
}
