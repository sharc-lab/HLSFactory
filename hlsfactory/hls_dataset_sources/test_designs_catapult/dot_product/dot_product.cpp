int dot_product(const short a[8], const short b[8]) {
    int result = 0;
    for (int i = 0; i < 8; ++i) {
        result += a[i] * b[i];
    }
    return result;
}
