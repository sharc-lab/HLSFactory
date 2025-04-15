## mvt

Matrix vector multiplication composed with another matrix vector multiplication but with transposed matrix.
It takes the following as inputs,

- $A: N \times N$ matrix
- $y1, y2$: vectors of length $N$

and gives the following as outputs:

- $x1$: vector of length $N$, where $x1 = x1 + Ay1$
- $x2$: vector of length $N$, where $x2 = x2 + A^Ty2$