## cholesky

Cholesky decomposition, which decomposes a matrix to triangular matrices. Only applicable when the input matrix is positive-definite.
It takes the following as input,

- $A: N \times N$ positive-definite matrix

and gives the following as output:

- $L: N \times N$ lower triangular matrix such that $A = LL^T$

The C reference implementation uses CholeskyBanachiewicz algorithm. The algorithm computes the following, where the computation starts from the upper-left corner of $L$ and proceeds row by row.

$$
L(i,j) =
\begin{cases}
\sqrt{A(i, i) - \sum_{k=0}^{i-1} L(i, k)^2} & \text{for } i = j \\
\frac{1}{L(j, j)} \left( A(i, j) - \sum_{k=0}^{j-1} L(i, k)L(j, k) \right) & \text{for } i > j
\end{cases}
$$