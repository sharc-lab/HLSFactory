## gramschmidt

QR Decomposition with Modified Gram Schmidt.
It takes the following as input,

- $A: M \times N$ rank $N$ matrix ($M \geq N$).

and gives the following as outputs:

- $Q: M \times N$ orthogonal matrix
- $R: N \times N$ upper triangular matrix

such that $A = QR$.
The algorithm is described in a techreport by Walter Gander: `http://www.inf.ethz.ch/personal/gander/`.