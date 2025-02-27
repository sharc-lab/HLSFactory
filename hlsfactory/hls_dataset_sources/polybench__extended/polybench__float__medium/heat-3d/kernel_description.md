## heat-3d

Heat Equation over 3D space. The main update is as follows:

$$
\begin{align*}
data^{t}_{(i,j,k)} = & \ data^{t-1}_{(i,j,k)} \\
& + 0.125 \cdot \left( data^{t-1}_{(i+1,j,k)} - 2 \cdot data^{t-1}_{(i,j,k)} + data^{t-1}_{(i-1,j,k)} \right) \\
& + 0.125 \cdot \left( data^{t-1}_{(i,j+1,k)} - 2 \cdot data^{t-1}_{(i,j,k)} + data^{t-1}_{(i,j-1,k)} \right) \\
& + 0.125 \cdot \left( data^{t-1}_{(i,j,k+1)} - 2 \cdot data^{t-1}_{(i,j,k)} + data^{t-1}_{(i,j,k-1)} \right)
\end{align*}
$$

The C reference implementation is originally from Pochoir distribution.