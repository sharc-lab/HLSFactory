## ludcmp

This kernel solves a system of linear equations using LU decomposition followed by forward and backward substitutions.
It takes the following as inputs,

- $A: N \times N$ matrix
- $b$: vector of length $N$

and gives the following as output:

- $x$: vector of length $N$, where $Ax = b$

The matrix $A$ is first decomposed into $L$ and $U$ using the same algorithm as in `lu`. Then the two triangular systems are solved to find $x$ as follows:

$$
Ax = b \Rightarrow LUx = b \Rightarrow
\begin{cases}
Ly = b \\
Ux = y
\end{cases}
$$

The forward and backward substitutions are as follows:

$$
y(i) = \frac{b(i) - \sum_{j=0}^{i-1} L(i,j)y(j)}{L(i,i)}
$$

$$
x(i) = \frac{y(i) - \sum_{j=0}^{i-1} U(i,j)x(j)}{U(i,i)}
$$