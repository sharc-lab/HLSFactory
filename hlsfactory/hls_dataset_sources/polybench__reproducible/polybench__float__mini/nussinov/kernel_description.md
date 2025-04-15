## nussinov

Nussinov is an algorithm for predicting RNA folding, and is an instance of dynamic programming.
It takes the following as input,

- `seq`: RNA sequence of length $N$. The valid entries are one of 'A' 'G' 'C' 'T'. (or 'U' in place of 'T').

and gives the following as output:

- `table`: $N \times N$ triangular matrix, which is the dynamic programming table.

The table is filled using the following formula:

$$
table(i,j) = \max
\begin{cases}
table(i+1,j) \\
table(i,j-1) \\
table(i+1,j-1) + w(i,j) \\
\max_{i < k < j}(table(i,k) + table(k+1,j))
\end{cases}
$$

where $w$ is the scoring function that evaluate the pair of sequences $seq[i]$ and $seq[j]$. For Nussinov algorithm, the scoring function returns 1 if the sequences are complementary (either 'A' with 'T' or 'G' with 'C'), and 0 otherwise.