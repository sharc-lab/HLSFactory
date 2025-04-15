## fdtd-2d

Simplified Finite-Difference Time-Domain method for 2D data, which models electric and magnetic fields based on Maxwell’s equations. In particular, the polarization used here is \( TE^z \); Transverse Electric in \( z \) direction. It is a stencil involving three variables, \( Ex \), \( Ey \), and \( Hz \). \( Ex \) and \( Ey \) are electric fields varying in \( x \) and \( y \) axes, where \( Hz \) is the magnetic field along \( z \) axis. Fields along other axes are either zero or static, and are not modeled.

$$
\begin{align*}
Hz_{(i,j)}^{t} &= C_{hzh} Hz_{(i,j)}^{t-1} + C_{hze} \left(Ex_{(i,j+1)}^{t-1} - Ex_{(i,j)}^{t-1} - Ey_{(i+1,j)}^{t-1} + Ey_{(i,j)}^{t-1}\right) \\
Ex_{(i,j)}^{t} &= C_{exe} Ex_{(i,j)}^{t-1} + C_{exh} \left(Hz_{(i,j)}^{t} - Hz_{(i,j-1)}^{t}\right) \\
Ey_{(i,j)}^{t} &= C_{eye} Ey_{(i,j)}^{t-1} + C_{eyh} \left(Hz_{(i,j)}^{t} - Hz_{(i-1,j)}^{t}\right)
\end{align*}
$$

Variables \( C_{xxx} \) are coefficients that may be different depending on the location within the discretized space. In PolyBench, it is simplified as scalar coefficients.