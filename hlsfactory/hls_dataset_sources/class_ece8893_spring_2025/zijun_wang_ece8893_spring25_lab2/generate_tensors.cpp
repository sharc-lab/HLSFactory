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

    // Allocate memory for tensors
    fixed_t *Q = new fixed_t[B * N * dk];
    fixed_t *K = new fixed_t[B * N * dk];
    fixed_t *V = new fixed_t[B * N * dv];

    // Generate random values and populate tensors within -1 and 1
    srand(42); // Seed for reproducibility
    for (int i = 0; i < B * N * dk; ++i) {
        Q[i] = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
        K[i] = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
    }
    for (int i = 0; i < B * N * dv; ++i) {
        V[i] = (fixed_t)(static_cast<float>(rand()) / RAND_MAX * 2 - 1); // Scale to [-1, 1]
    }

    // Write tensors to binary files
    Q_file.write(reinterpret_cast<char*>(Q), B * N * dk * sizeof(fixed_t));
    K_file.write(reinterpret_cast<char*>(K), B * N * dk * sizeof(fixed_t));
    V_file.write(reinterpret_cast<char*>(V), B * N * dv * sizeof(fixed_t));

    // Clean up
    delete[] Q;
    delete[] K;
    delete[] V;

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
