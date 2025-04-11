## gesummv

Summed matrix-vector multiplications from updated BLAS. However, it is not part of the current BLAS distribution.
It takes the following as inputs,

- $\alpha, \beta$: scalars
- $A, B$: $N \times N$ matrix
- $x$: vector of length $N$

and gives the following as outputs:

- $y$: vector of length $N$, where $y = \alpha Ax + \beta Bx$