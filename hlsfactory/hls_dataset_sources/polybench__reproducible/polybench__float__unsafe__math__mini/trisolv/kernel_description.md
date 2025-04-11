## trisolv

Triangular matrix solver using forward substitution.
It takes the following as inputs,

- $L$: $N \times N$ lower triangular matrix
- $b$: vector of length $N$

and gives the following as output:

- $x$: vector of length $N$, where $Lx = b$

The forward substitution is as follows:

$$
x(i) = \frac{b(i) - \sum_{j=0}^{i-1} L(i, j) \cdot x(j)}{L(i, i)}
$$