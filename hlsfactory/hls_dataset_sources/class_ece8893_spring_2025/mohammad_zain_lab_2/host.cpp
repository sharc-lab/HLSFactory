#include "dcl.h"

using namespace std;
// Packing Q and K to a 512 bits type for wide bus transaction
void pack_variable_3D_to_wide(fixed_t Variable_16_bit[B][N][dk], wide_bus_t Variable_wide[B][N][dk/32]) {
    for (int b = 0; b < B; b++) {
        for (int i = 0; i < N; i++) {
            for (int k = 0; k < dk/32; k++) { //each element is 16 bits, so 32 elements in 512 bits. So need to run 128/32 times to store all
                wide_bus_t packed_data = 0; //create a local variable to store 512 bits
                for (int sub_k = 0; sub_k < 32; sub_k++) {
                    //starts with 0 to 16, then 16 to 32, then 32 to 48, then 48 to 64
                    packed_data.range(((sub_k + 1) * 16) - 1, sub_k*16) = Variable_16_bit[b][i][sub_k + k*32];
                }
                Variable_wide[b][i][k] = packed_data;
            }
        }
    }
}

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
    wide_bus_t Q_wide_bus[B][N][dk/32];
    wide_bus_t K_wide_bus[B][N][dk/32];
    wide_bus_t V_wide_bus[B][N][dv/32];


    // Load tensors from binary files

    load_tensor("Q_tensor.bin", Q, dk);
    load_tensor("K_tensor.bin", K, dk);
    load_tensor("V_tensor.bin", V, dv);
	load_tensor("Output_tensor.bin", Output_ref, dv);


    //pack to wide bus
    pack_variable_3D_to_wide(Q, Q_wide_bus);
    pack_variable_3D_to_wide(K, K_wide_bus);
    pack_variable_3D_to_wide(V, V_wide_bus);

	for(int i = 0; i < B; i++) {
		for(int j = 0; j < N; j++) {
			for(int k = 0; k < dv; k++) {
				Output_HLS[i][j][k] = 0;
			}
		}
	}

    // call HLS kernel
    compute_attention_HLS(Q_wide_bus, K_wide_bus, V_wide_bus, Output_HLS);
	
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
