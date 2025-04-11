## doitgen

Kernel of Multiresolution ADaptive NumErical Scientific Simulation (MADNESS). The kernel is taken from the modified version used by You et al.
It takes the following as inputs,

- $A: R \times Q \times S$ array
- $x: P \times S$ array

and gives the following as output:

- $A_{out}: R \times Q \times P$ array

where $A_{out}(r, q, p) = \sum_{s=0}^{S-1} A(r, q, s) x(p, s)$

Note that the output $A_{out}$ is to be stored in place of the input array $A$ in the original code. Although it is not mentioned anywhere, the computation does not make sense if $P \neq S$.