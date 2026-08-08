Kernel Description:
The ShiftRows kernel is a crucial component of the Advanced Encryption Standard (AES) algorithm, responsible for shifting the rows of the state matrix to the left by different offsets. The state matrix is a 4x4 array of bytes, where each row represents a 32-bit word. The ShiftRows operation is performed by rotating each row of the state matrix to the left by a number of columns equal to the row number, with the first row (row 0) not being shifted. This operation is a key part of the AES encryption process, as it helps to diffuse the plaintext data across the state matrix, making it more difficult to recover the original data without the encryption key. The ShiftRows operation can be represented mathematically as follows: 
\[
\begin{bmatrix}
a_{0,0} & a_{0,1} & a_{0,2} & a_{0,3} \\
a_{1,0} & a_{1,1} & a_{1,2} & a_{1,3} \\
a_{2,0} & a_{2,1} & a_{2,2} & a_{2,3} \\
a_{3,0} & a_{3,1} & a_{3,2} & a_{3,3} \\
\end{bmatrix}
\rightarrow
\begin{bmatrix}
a_{0,0} & a_{0,1} & a_{0,2} & a_{0,3} \\
a_{1,1} & a_{1,2} & a_{1,3} & a_{1,0} \\
a_{2,2} & a_{2,3} & a_{2,0} & a_{2,1} \\
a_{3,3} & a_{3,0} & a_{3,1} & a_{3,2} \\
\end{bmatrix}
\]
where $a_{i,j}$ represents the byte at row $i$ and column $j$ of the state matrix.

---

Top-Level Function: `ShiftRows`

Complete Function Signature of the Top-Level Function:
`void ShiftRows(state_t *state);`

Inputs:
- `state`: a pointer to a 4x4 array of bytes, representing the state matrix to be shifted.

Outputs:
- None, the ShiftRows operation is performed in-place on the input state matrix.

Important Data Structures and Data Types:
- `state_t`: a 4x4 array of bytes, representing the state matrix.

Sub-Components:
- None