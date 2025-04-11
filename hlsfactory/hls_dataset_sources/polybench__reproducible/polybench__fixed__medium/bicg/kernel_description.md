## bicg

Kernel of BiCGSTAB (BiConjugate Gradient STABilized method).
It takes the following as inputs,

- $A: N \times M$ matrix
- $p$: vector of length $M$
- $r$: vector of length $N$

and gives the following as output:

- $q$: vector of length $N$, where $q = Ap$
- $s$: vector of length $M$, where $s = A^Tr$