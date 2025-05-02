#include "dcl.h"

using namespace std;

// Read a tensor from a file
void load_tensor(const char* filename, float tensor[][K][L]) {
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error opening file: %s\n", filename);
        exit(1);
    }

    size_t elements_read = fread(tensor, sizeof(float), BS * K * L, file);
    if (elements_read != BS * K * L) {
        fprintf(stderr, "Error reading file: %s\n", filename);
        fclose(file);
        exit(1);
    }

    fclose(file);
}

int main() {

    float A[BS][K][L];
    float B[BS][K][L];
    float C_ref[BS][K][L];
    float C_HLS[BS][K][L];

    load_tensor("input1_A.bin", A);
    load_tensor("input1_B.bin", B);
    load_tensor("output1_C.bin", C_ref);

    for(int b = 0; b < BS; b++) {
		for(int k = 0; k < K; k++) {
			for(int l = 0; l < L; l++) {
				C_HLS[b][k][l] = 0;
			}
		}
	}

    // Initialize wide bus transaction matrices
    mem_t A_mem[BS][K][L/MS];
    mem_t B_mem[BS][K][L/MS];
    mem_t C_HLS_mem[BS][K][L/MS];

    // Copy values into wide bus transaction matrices
    for (int b = 0; b < BS; b++) {
        for (int k = 0; k < K; k++) {
            for (int l = 0; l < L/MS; l++) {
                for (int i = 0; i < MS; i++) {
                    ap_uint<32> tmp;
                    memcpy(&tmp, &A[b][k][l * (L/MS) + i], sizeof(float));
                    A_mem[b][k][l].range(31 + (i * 32), 0 + (i * 32)) = tmp;
                    memcpy(&tmp, &B[b][k][l * (L/MS) + i], sizeof(float));
                    B_mem[b][k][l].range(31 + (i * 32), 0 + (i * 32)) = tmp;
                    memcpy(&tmp, &C_HLS[b][k][l * (L/MS) + i], sizeof(float));
                    C_HLS_mem[b][k][l].range(31 + (i * 32), 0 + (i * 32)) = tmp;
                }
            }
        }
    }

    // Call HLS kernel to perform symbolic circular convolution
    symbolic_circular_conv_HLS(A_mem, B_mem, C_HLS_mem);

    // Convert resulting C matrix back into standard format
    for (int b = 0; b < BS; b++) {
        for (int k = 0; k < K; k++) {
            for (int l = 0; l < L / MS; l++) {
                for (int i = 0; i < MS; i++) {
                    ap_uint<32> tmp = C_HLS_mem[b][k][l].range(31 + (i * 32), 0 + (i * 32));
                    memcpy(&C_HLS[b][k][l * (L / MS) + i], &tmp, sizeof(float));
                }
            }
        }
    }
 	
	// compare HLS output and reference output tensor
    float error = 0;
	for(int b = 0; b < BS; b++) {
		for(int k = 0; k < K; k++) {
			for(int l = 0; l < L; l++) {
				error += std::pow(C_HLS[b][k][l] - C_ref[b][k][l], 2);
			}
		}
	}
	error = error / (BS * K * L);
	printf("MSE: %.8f\n", error);

    return 0;
}