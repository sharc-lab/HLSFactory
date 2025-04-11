## correlation

Correlation computes the correlation coefficients (Pearson's), which is normalized covariance.
It takes the following as input,

- `data`: $N \times M$ matrix that represents $N$ data points, each with $M$ attributes,

and gives the following as output:

- `corr`: $M \times M$ matrix where the $i,j$-th element is the correlation coefficient between $i$ and $j$. The matrix is symmetric.

Correlation is defined as the following,

$$
\text{corr}(i,j) = \frac{\text{cov}(i,j)}{\text{stddev}(i)\text{stddev}(j)}
$$

where

$$
\text{stddev}(x) = \sqrt{\frac{\sum_{k=0}^{N-1} (\text{data}(k, x) - \text{mean}(x))^2}{N}}
$$

`cov` and `mean` are defined in covariance (1).

(1): However, the denominator when computing covariance is $N$ for correlation.