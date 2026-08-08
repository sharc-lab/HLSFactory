Kernel Description:
The SHA256 update kernel is a high-level synthesis design that implements the SHA-256 hash function. The kernel takes in a block of data and updates the internal state of the hash function. The kernel is designed to be highly efficient and scalable, making it suitable for use in a variety of applications.

Top-Level Function: `sha256_update`
Complete Function Signature of the Top-Level Function: `void sha256_update(data_t *data_int, unsigned int *datalen_int, state_t *state, unsigned long long int *bitlen_int, data_t data[], size_t len);`

Inputs:
- `data_int`: a pointer to a 64-byte array representing the input data
- `datalen_int`: a pointer to an unsigned integer representing the length of the input data
- `state`: a pointer to an 8-element array of unsigned integers representing the internal state of the hash function
- `bitlen_int`: a pointer to an unsigned long long integer representing the total number of bits processed
- `data`: a 64-byte array representing the input data
- `len`: a size_t representing the length of the input data

Outputs:
- None

Important Data Structures and Data Types:
- `data_t`: a 64-byte array representing the input data
- `state_t`: an 8-element array of unsigned integers representing the internal state of the hash function
- `SHA256_CTX`: a struct representing the context of the hash function, containing the input data, data length, bit length, and internal state

Sub-Components:
- `sha256_init`:
    - Signature: `void sha256_init(SHA256_CTX *ctx);`
    - Details: initializes the hash function context
- `sha256_transform`:
    - Signature: `void sha256_transform(state_t *state, data_t data[]);`
    - Details: performs the core hash function computation
- `sha256_final`:
    - Signature: `void sha256_final(SHA256_CTX *ctx, unsigned char hash[]);`
    - Details: finalizes the hash function computation and returns the resulting hash value