## symm

Symmetric matrix multiplication from BLAS.
It takes the following as inputs,

- $\alpha, \beta$: scalars
- $A$: $M \times M$ symmetric matrix
- $B, C$: $M \times N$ matrices

and gives the following as output:

- $C_{\text{out}}$: $M \times N$ matrix, where $C_{\text{out}} = \alpha AB + \beta BC$

Note that the output $C_{\text{out}}$ is to be stored in place of the input array $C$. The matrix $A$ is stored as a triangular matrix in BLAS. The configuration used are `SIDE = 'L'` and `UPLO = 'L'`, meaning the multiplication is from the left, and the symmetric matrix is stored as a lower triangular matrix.