## covariance

Computes the covariance, a measure from statistics that show how linearly related two variables are.
It takes the following as input,

- `data`: $N \times M$ matrix that represents $N$ data points, each with $M$ attributes,

and gives the following output:

- `cov`: $M \times M$ matrix where the $i,j$-th element is the covariance between $i$ and $j$. The matrix is symmetric.

Covariance is defined to be the mean of the product of deviations for $i$ and $j$:

$$
\text{cov}(i,j) = \frac{\sum_{k=0}^{N-1} ( \text{data}(k,i) - \text{mean}(i) )(\text{data}(k,j) - \text{mean}(j) )}{N - 1}
$$

where

$$
\text{mean}(x) = \frac{\sum_{k=0}^{N-1} \text{data}(k, x)}{N}
$$

Note that the above computes *sample covariance* where the denominator is $N - 1$.