## gemm

Generalized Matrix Multiply from BLAS.
It takes the following as inputs,

- $\alpha, \beta$: scalars
- $A$: $P \times Q$ matrix
- $B$: $Q \times R$ matrix
- $C$: $P \times R$ matrix

and gives the following as output:

- $C_{out}$: $P \times R$ array, where $C_{out} = \alpha AB + \beta C$

Note that the output $C_{out}$ is to be stored in place of the input array $C$. The BLAS parameters used are `TRANSA = TRANSB = ‘N’`, meaning both $A$ and $B$ are not transposed.