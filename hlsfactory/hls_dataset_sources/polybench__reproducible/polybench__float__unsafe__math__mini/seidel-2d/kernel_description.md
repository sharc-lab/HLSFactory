## seidel-2d

Gauss-Seidel style stencil computation over 2D data with 9-point stencil pattern. Similar to Jacobi style stencils, Gauss-Seidel style stencil comes from Gauss-Seidel method for solving systems of linear equations. The main difference is that values computed at the same time step is used, which changes its convergence and other properties. The computation in PolyBench is simplified as simply taking the average of nine points.

$$
\begin{aligned}
data^{t}_{(i,j)} = \frac{1}{9} \big(
& data^{t-1}_{(i-1,j-1)} + data^{t-1}_{(i-1,j)} + data^{t-1}_{(i-1,j+1)} \\
& + data^{t-1}_{(i,j-1)} + data^{t}_{(i,j)} + data^{t-1}_{(i,j+1)} \\
& + data^{t-1}_{(i+1,j-1)} + data^{t-1}_{(i+1,j)} + data^{t-1}_{(i+1,j+1)}
\big)
\end{aligned}
$$