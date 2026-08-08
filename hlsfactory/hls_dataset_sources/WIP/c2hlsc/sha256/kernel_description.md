Kernel Description:
The SHA-256 kernel is a cryptographic hash function that takes input data of any size and produces a fixed-size, 256-bit (32-byte) hash value. The kernel is designed to be secure, meaning that it is computationally infeasible to find two different input messages with the same hash value. The kernel uses a Merkle-Damgård construction, which consists of a compression function and a message padding scheme. The compression function takes a 256-bit message block and a 256-bit state, and produces a new 256-bit state. The message padding scheme pads the input message to a multiple of the block size, and then appends a 64-bit representation of the message length.

The kernel uses a series of bitwise operations, including rotations, shifts, and XORs, to mix the input data and state. The kernel also uses a set of predefined constants, known as the round constants, which are used to introduce randomness into the hash value. The kernel iterates over the input data in 64-byte blocks, processing each block in a series of 64 rounds. In each round, the kernel performs a set of operations on the state and the current block of input data, using the round constants to introduce randomness.

The kernel also includes a finalization step, which pads the last block of input data and appends the message length. The finalization step ensures that the kernel produces a fixed-size hash value, regardless of the size of the input message.

The kernel uses the following equations to perform the compression function:
\[ t_1 = h + \Sigma_1(e) + Ch(e, f, g) + k_i + m_i \]
\[ t_2 = \Sigma_0(a) + Maj(a, b, c) \]
\[ h = g \]
\[ g = f \]
\[ f = e \]
\[ e = d + t_1 \]
\[ d = c \]
\[ c = b \]
\[ b = a \]
\[ a = t_1 + t_2 \]
where $h, g, f, e, d, c, b, a$ are the state variables, $k_i$ is the round constant, $m_i$ is the input message block, and $\Sigma_0, \Sigma_1, Ch, Maj$ are the bitwise operations.

The kernel also uses the following equations to perform the message padding:
\[ m = m || 0x80 || 0x00^{*} || l \]
where $m$ is the input message, $0x80$ is the padding byte, $0x00^{*}$ is the padding zeros, and $l$ is the message length.

---

Top-Level Function: `sha256_update`

Complete Function Signature of the Top-Level Function:
`void sha256_update(data_t *data_int, unsigned int *datalen_int, state_t *state, unsigned long long int *bitlen_int, data_t data[], size_t len);`

Inputs:
- `data_int`: a pointer to the current block of input data
- `datalen_int`: a pointer to the current length of the input data
- `state`: a pointer to the current state of the kernel
- `bitlen_int`: a pointer to the current length of the input message in bits
- `data`: the input data to be processed
- `len`: the length of the input data

Outputs:
- None

Important Data Structures and Data Types:
- `data_t`: an array of 64 unsigned characters, representing a block of input data
- `state_t`: an array of 8 unsigned integers, representing the state of the kernel
- `SHA256_CTX`: a structure containing the current state of the kernel, including the input data, length, and bit length

Sub-Components:
- `sha256_transform`:
    - Signature: `void sha256_transform(state_t *state, data_t *data);`
    - Details: performs the compression function on the input data and state, producing a new state
- `sha256_init`:
    - Signature: `void sha256_init(SHA256_CTX *ctx);`
    - Details: initializes the kernel state and input data
- `sha256_final`:
    - Signature: `void sha256_final(SHA256_CTX *ctx, unsigned char hash[]);`
    - Details: performs the finalization step, padding the last block of input data and appending the message length, and produces the final hash value