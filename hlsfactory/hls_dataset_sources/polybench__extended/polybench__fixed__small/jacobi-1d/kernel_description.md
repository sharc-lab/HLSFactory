## jacobi-1d

Jacobi style stencil computation over 1D data with 3-point stencil pattern. It originally comes from the Jacobi method for solving system of equations. However, Jacobi-style stencils may refer to computations with some stencil pattern that use values computed in the previous time step, which is a characteristic of Jacobi method. The computation in PolyBench is simplified as simply taking the average of three points.

$$
data^{t}_{(i)} = \frac{1}{3} \left( data^{t-1}_{(i)} + data^{t-1}_{(i-1)} + data^{t-1}_{(i+1)} \right)
$$