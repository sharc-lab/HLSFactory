// VECTOR_SIZE and SCALE are supplied by CPPPreprocessorFrontend as -D options.
void vector_affine(const int a[VECTOR_SIZE], const int b[VECTOR_SIZE],
                   int out[VECTOR_SIZE]) {
    for (int i = 0; i < VECTOR_SIZE; ++i) {
#pragma HLS PIPELINE II=1
        out[i] = SCALE * a[i] + b[i];
    }
}
