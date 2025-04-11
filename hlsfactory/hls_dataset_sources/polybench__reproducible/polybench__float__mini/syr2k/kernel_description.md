## syr2k

Symmetric rank 2k update from updated BLAS.
It takes the following as inputs,

- $\alpha, \beta$: scalars
- $A, B$: $N \times M$ matrices
- $C$: $N \times N$ symmetric matrix

and gives the following as output:

- $C_{out}$: $N \times N$ matrix, where $C_{out} = \alpha AB^T + \alpha BA^T + \beta C$

Note that the output $C_{out}$ is to be stored in place of the input array $C$. The matrix $C$ is stored as a triangular matrix in BLAS, and the result is also triangular. The configurations used are $TRANS = 'N'$ and $UPLO = 'L'$ meaning the $A$ matrix is not transposed, and $