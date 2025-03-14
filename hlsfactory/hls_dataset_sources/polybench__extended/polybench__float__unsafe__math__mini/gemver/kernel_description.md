## gemver

Multiple matrix-vector multiplication from updated BLAS. However, it is not part of the current BLAS distribution.
It takes the following as inputs,

- $\alpha, \beta$: scalars
- $A$: $N \times N$ matrix
- $u1, v1, u2, v2, y, z$: vectors of length $N$

and gives the following as outputs:

- $A$: $N \times N$ matrix, where $A = A + u1 \cdot v1^T + u2 \cdot v2^T$
- $x$: vector of length $N$, where $x = \beta A^T y + z$
- $w$: vector of length $N$, where $w = \alpha A x$