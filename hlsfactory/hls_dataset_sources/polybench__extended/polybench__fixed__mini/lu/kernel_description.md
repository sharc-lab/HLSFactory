## lu

LU decomposition without pivoting.
It takes the following as input,

- $A: N \times N$ matrix

and gives the following as outputs:

- $L: N \times N$ lower triangular matrix
- $U: N \times N$ upper triangular matrix

such that $A = LU$.
L and U are computed as follows:

$$
U(i,j) = A(i,j) - \sum_{k=0}^{i-1} L(i,k)U(k,j)
$$

$$
L(i,j) = \frac{1}{U(j,j)} \left( A(i,j) - \sum_{k=0}^{j-1} L(i,k)U(k,j) \right)
$$