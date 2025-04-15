## jacobi-2d

Jacobi-style stencil computation over 2D data with 5-point stencil pattern. The computation is simplified as simply taking the average of five points.

$$
data^{t}_{(i,j)} = \frac{1}{5} \left( data^{t-1}_{(i,j)} + data^{t-1}_{(i-1,j)} + data^{t-1}_{(i+1,j)} + data^{t-1}_{(i,j-1)} + data^{t-1}_{(i,j+1)} \right)
$$