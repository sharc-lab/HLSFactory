Kernel Description:
The AES (Advanced Encryption Standard) kernel is a high-level synthesis design that implements the AES encryption algorithm. The algorithm is a symmetric-key block cipher that encrypts and decrypts data in blocks of 128 bits. The kernel takes a 128-bit plaintext block and a 128-bit key as input and produces a 128-bit ciphertext block as output. The AES algorithm consists of multiple rounds of substitution, permutation, and key mixing, with the number of rounds depending on the key size. In this implementation, the key size is 128 bits, which requires 10 rounds of encryption.

The kernel uses a combination of bitwise operations, table lookups, and modular arithmetic to perform the encryption. The encryption process involves four stages: key expansion, substitution, permutation, and mixing. The key expansion stage generates a set of round keys from the input key, which are used in each round of encryption. The substitution stage replaces each byte of the plaintext with a corresponding byte from a substitution box (S-box). The permutation stage rearranges the bytes of the plaintext, and the mixing stage combines the permuted bytes with the round keys.

The kernel uses several data structures, including the state matrix, which represents the plaintext or ciphertext block, and the round key array, which stores the round keys generated during key expansion. The kernel also uses several sub-components, including the KeyExpansion function, which generates the round keys, and the Cipher function, which performs the encryption.

The AES algorithm can be mathematically represented using the following equations:

* SubBytes: $S(x) = sbox[x]$
* ShiftRows: $SR(x) = (x_{0,0}, x_{1,1}, x_{2,2}, x_{3,3})$
* MixColumns: $MC(x) = (x_{0,0} \oplus x_{1,1} \oplus x_{2,2} \oplus x_{3,3}, x_{0,1} \oplus x_{1,2} \oplus x_{2,3} \oplus x_{3,0}, x_{0,2} \oplus x_{1,3} \oplus x_{2,0} \oplus x_{3,1}, x_{0,3} \oplus x_{1,0} \oplus x_{2,1} \oplus x_{3,2})$
* AddRoundKey: $ARK(x, k) = x \oplus k$

Where $x$ represents the state matrix, $sbox$ represents the substitution box, and $k$ represents the round key.

---

Top-Level Function: `Cipher`

Complete Function Signature of the Top-Level Function:
`void Cipher(state_t *state, const round_t *RoundKey);`

Inputs:
- `state`: a 4x4 matrix representing the plaintext or ciphertext block, where each element is an 8-bit unsigned integer.
- `RoundKey`: an array of 176 8-bit unsigned integers representing the round keys generated during key expansion.

Outputs:
- The encrypted or decrypted block is stored in the `state` matrix.

Important Data Structures and Data Types:
- `state_t`: a 4x4 matrix of 8-bit unsigned integers representing the plaintext or ciphertext block.
- `round_t`: an array of 176 8-bit unsigned integers representing the round keys generated during key expansion.
- `AES_ctx`: a structure containing the round key array and other context information.

Sub-Components:
- `KeyExpansion`:
    - Signature: `void KeyExpansion(uint8_t *RoundKey, const uint8_t *Key);`
    - Details: generates the round keys from the input key.
- `AddRoundKey`:
    - Signature: `void AddRoundKey(uint8_t round, state_t *state, const round_t *RoundKey);`
    - Details: adds the round key to the state matrix.
- `SubBytes`:
    - Signature: `void SubBytes(state_t *state);`
    - Details: replaces each byte of the state matrix with a corresponding byte from the substitution box.
- `ShiftRows`:
    - Signature: `void ShiftRows(state_t *state);`
    - Details: rearranges the bytes of the state matrix.
- `MixColumns`:
    - Signature: `void MixColumns(state_t *state);`
    - Details: combines the permuted bytes with the round keys.