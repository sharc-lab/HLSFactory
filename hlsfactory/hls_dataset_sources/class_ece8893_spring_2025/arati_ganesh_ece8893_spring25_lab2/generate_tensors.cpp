#include "dcl.h"

using namespace std;

void generate_attention_matrices() {
    // Open binary files for writing
    ofstream Q_file("Q_tensor.bin", ios::binary);
    ofstream K_file("K_tensor.bin", ios::binary);
    ofstream V_file("V_tensor.bin", ios::binary);

    if (!Q_file || !K_file || !V_file) {
        cerr << "Error opening file for writing." << endl;
        exit(1);
    }

#ifdef WIDE_BUS
    // Allocate memory for tensors in wide format
    wide_t *Q = new wide_t[B * N * (dk / ELEMENTS_PER_WIDE)];
    wide_t *K = new wide_t[B * N * (dk / ELEMENTS_PER_WIDE)];
    wide_t *V = new wide_t[B * N * (dv / ELEMENTS_PER_WIDE)];
#else
    // Allocate memory for tensors in fixed_t format
    fixed_t *Q = new fixed_t[B * N * dk];
    fixed_t *K = new fixed_t[B * N * dk];
    fixed_t *V = new fixed_t[B * N * dv];
#endif

    // Generate random values and populate tensors within -1 and 1
    srand(42); // Seed for reproducibility
#ifdef WIDE_BUS
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < dk / ELEMENTS_PER_WIDE; ++j) {
                wide_t Q_wide, K_wide;
                for (int k = 0; k < ELEMENTS_PER_WIDE; ++k) {
                    fixed_t Q_val = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
                    fixed_t K_val = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
                    Q_wide.range(32 * (k + 1) - 1, 32 * k) = Q_val;
                    K_wide.range(32 * (k + 1) - 1, 32 * k) = K_val;
                }
                Q[b * N * (dk / ELEMENTS_PER_WIDE) + i * (dk / ELEMENTS_PER_WIDE) + j] = Q_wide;
                K[b * N * (dk / ELEMENTS_PER_WIDE) + i * (dk / ELEMENTS_PER_WIDE) + j] = K_wide;
            }
        }
    }
    for (int b = 0; b < B; ++b) {
        for (int i = 0; i < N; ++i) {
            for (int j = 0; j < dv / ELEMENTS_PER_WIDE; ++j) {
                wide_t V_wide;
                for (int k = 0; k < ELEMENTS_PER_WIDE; ++k) {
                    fixed_t V_val = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
                    V_wide.range(32 * (k + 1) - 1, 32 * k) = V_val;
                }
                V[b * N * (dv / ELEMENTS_PER_WIDE) + i * (dv / ELEMENTS_PER_WIDE) + j] = V_wide;
            }
        }
    }
#else
    for (int i = 0; i < B * N * dk; ++i) {
        Q[i] = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
        K[i] = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
    }
    for (int i = 0; i < B * N * dv; ++i) {
        V[i] = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
    }
#endif

    // Write tensors to binary files
#ifdef WIDE_BUS
    Q_file.write(reinterpret_cast<char*>(Q), B * N * (dk / ELEMENTS_PER_WIDE) * sizeof(wide_t));
    K_file.write(reinterpret_cast<char*>(K), B * N * (dk / ELEMENTS_PER_WIDE) * sizeof(wide_t));
    V_file.write(reinterpret_cast<char*>(V), B * N * (dv / ELEMENTS_PER_WIDE) * sizeof(wide_t));
#else
    Q_file.write(reinterpret_cast<char*>(Q), B * N * dk * sizeof(fixed_t));
    K_file.write(reinterpret_cast<char*>(K), B * N * dk * sizeof(fixed_t));
    V_file.write(reinterpret_cast<char*>(V), B * N * dv * sizeof(fixed_t));
#endif

    // Clean up
#ifdef WIDE_BUS
    delete[] Q;
    delete[] K;
    delete[] V;
#else
    delete[] Q;
    delete[] K;
    delete[] V;
#endif

    Q_file.close();
    K_file.close();
    V_file.close();

    cout << "Generated tensors saved in binary format:" << endl;
    cout << "Q: " << B << "x" << N << "x" << dk << endl;
    cout << "K: " << B << "x" << N << "x" << dk << endl;
    cout << "V: " << B << "x" << N << "x" << dv << endl;
}
int main() {

    // Generate and save tensors
    generate_attention_matrices();

    return 0;
}
