## floyd-warshall

Floyd-Warshall computes the shortest paths between each pair of nodes in a graph. The following only computes the shortest path lengths, which is a typical use of this method.
It takes the following as input,

- $w$: $N \times N$ matrix, where the $i, j$ entry represents the cost of taking an edge from $i$ to $j$. Set to infinity if there is no edge connecting $i$ to $j$.

and gives the following as output:

- $paths$: $N \times N$ matrix, where the $i, j$ entry represents the shortest path length from $i$ to $j$.

The shortest path lengths are computed recursively as follows:

$$
p(k, i, j) =
\begin{cases}
w(i, j) & \text{if } k = -1 \\
\min(p(k - 1, i, j), p(k - 1, i, k) + p(k - 1, k, j)) & \text{if } 0 \leq k < N
\end{cases}
$$

where the final output $paths(i, j) = p(N - 1, i, j)$.