## 2mm

Linear algebra kernel that consists of two matrix multiplications.
It takes the following as inputs,

- $\alpha, \beta$: scalars
- $A: P \times Q$ matrix
- $B: Q \times R$ matrix
- $C: R \times S$ matrix
- $D: P \times S$ matrix

and gives the following as output:

- $E: P \times S$ matrix, where $E = \alpha ABC + \beta D$