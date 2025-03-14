## trmm

Triangular matrix multiplication.
It takes the following as inputs,

- $A$: $N \times N$ lower triangular matrix
- $B$: $N \times N$ matrix

and gives the following as output:

- $B_{out}$: $N \times N$ matrix, where $B_{out} = AB$

Note that the output $B_{out}$ is to be stored in place of the input array $B$. The configurations used are `SIDE = 'L'`, `UPLO = 'L'`, `TRANSA = 'T'`, and `DIAG = 'U'`, meaning the multiplication is from the left, the matrix is lower triangular, untransposed with unit diagonal.