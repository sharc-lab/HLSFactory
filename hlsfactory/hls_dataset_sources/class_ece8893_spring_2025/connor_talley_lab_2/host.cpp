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
	

    hls::vector<fixed_t, dk> Q_vec[B][N];
    hls::vector<fixed_t, dk> K_vec[B][N];
    hls::vector<fixed_t, dv> V_vec[B][N];
    hls::vector<fixed_t, dv> Output_HLS[B][N];


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

    //copy all data into the vector version for the kernel
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < dk; ++j) {
                Q_vec[b][i][j] = Q[b][i][j];
                K_vec[b][i][j] = K[b][i][j];
            }
            for (int j = 0; j < dv; ++j) {
                V_vec[b][i][j] = V[b][i][j];
            }
        }
    }

    // call HLS kernel
    compute_attention_HLS(Q_vec, K_vec, V_vec, Output_HLS);
	
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
