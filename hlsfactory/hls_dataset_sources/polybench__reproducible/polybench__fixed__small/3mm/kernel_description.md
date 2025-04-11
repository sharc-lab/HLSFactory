## 3mm

Linear algebra kernel that consists of three matrix multiplications.
It takes the following as inputs,

- $A: P \times Q$ matrix
- $B: Q \times R$ matrix
- $C: R \times S$ matrix
- $D: S \times T$ matrix

and gives the following as output:

- $G: P \times T$ matrix, where $G = (A.B).(C.D)$