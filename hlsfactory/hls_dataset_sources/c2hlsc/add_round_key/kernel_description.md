Kernel Description:
The AddRoundKey kernel function is a crucial component of the Advanced Encryption Standard (AES) cipher, responsible for adding the round key to the state. This process is performed by an XOR operation between the state and the round key. The state is a 4x4 matrix of bytes, while the round key is a one-dimensional array of bytes. The round key is added to the state in a specific order, determined by the round number and the AES key schedule. The kernel function takes three inputs: the round number, the state, and the round key. It iterates over each element of the state and performs an XOR operation with the corresponding element of the round key. The result is stored back in the state. This process is repeated for each round of the AES cipher.

The algorithm can be represented by the following equation:
\[ state_{i,j} = state_{i,j} \oplus RoundKey_{(round \times Nb \times 4) + (i \times Nb) + j} \]
where $state_{i,j}$ is the element at row $i$ and column $j$ of the state, $RoundKey_{k}$ is the $k^{th}$ element of the round key, $round$ is the round number, and $Nb$ is the number of columns in the state.

The kernel function is designed to be efficient and scalable, with a simple and regular dataflow. The use of XOR operations allows for fast and efficient implementation in hardware. The kernel function is also designed to be flexible, allowing for different round numbers and key schedules to be used.

---

Top-Level Function: `AddRoundKey`

Complete Function Signature of the Top-Level Function:
`void AddRoundKey(uint8_t round, state_t *state, const uint8_t *RoundKey);`

Inputs:
- `round`: The round number, which determines the offset into the round key array. This is an 8-bit unsigned integer.
- `state`: The state, which is a 4x4 matrix of bytes. This is a pointer to a 2D array of uint8_t.
- `RoundKey`: The round key, which is a one-dimensional array of bytes. This is a pointer to a 1D array of uint8_t.

Outputs:
- None, the state is modified in-place.

Important Data Structures and Data Types:
- `state_t`: A 4x4 matrix of bytes, represented as a 2D array of uint8_t. This data structure is used to represent the state of the AES cipher.
- `uint8_t`: An 8-bit unsigned integer, used to represent the round number and the elements of the state and round key.

Sub-Components:
- None