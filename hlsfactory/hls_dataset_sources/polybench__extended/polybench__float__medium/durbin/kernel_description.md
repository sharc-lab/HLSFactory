## durbin

Durbin is an algorithm for solving Yule-Walker equations, which is a special case of Toeplitz systems.
It takes the following as input,

- $r$: vector of length $N$.

and gives the following as output:

- $y$: vector of length $N$

such that $Ty = -r$ where $T$ is a symmetric, unit-diagonal, Toeplitz matrix defined by the vector $[1,r_0, \ldots ,r_{N-1}]$.
The C reference implementation is a direct implementation of the algorithm described in a book by Golub and Van Loan. The book mentions that a vector can be removed to use less space, but the implementation retains this vector.