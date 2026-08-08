Kernel Description:
The `BlockFrequency` kernel is designed to compute a statistical measure based on the frequency of blocks in a binary matrix. The matrix is defined by the dimensions \( M \times N \), where each element is either 0 or 1. The kernel calculates the sum of squared deviations of the block frequencies from 0.5, which is a measure used in statistical tests to determine if the distribution of 1s and 0s in the matrix is uniform.

The algorithm works as follows:
1. Initialize a variable `sum` to 0.0. This variable will accumulate the sum of squared deviations.
2. Iterate over each row `i` of the matrix.
3. For each row, compute the sum of its elements (`blockSum`).
4. Calculate the proportion `pi` of 1s in the row by dividing `blockSum` by `M`.
5. Compute the deviation `v` of `pi` from 0.5.
6. Square the deviation `v` and add it to `sum`.
7. After processing all rows, store the final accumulated `sum` in the `result` variable.

---

Top-Level Function: `BlockFrequency`

Complete Function Signature of the Top-Level Function:
`void BlockFrequency(double *result, int epsilon[M * N]);`

Inputs:
- `result`: A pointer to a double where the computed sum of squared deviations will be stored. The data type is `double` to accommodate the precision of the statistical measure.
- `epsilon`: A 2D array of integers with dimensions \( M \times N \) (flattened into a 1D array of size \( M \times N \)). Each element in the array is either 0 or 1, representing the binary matrix.

Outputs:
- `result`: The computed sum of squared deviations of block frequencies from 0.5, stored in the location pointed to by the `result` pointer.

Important Data Structures and Data Types:
- `epsilon`: A 1D array of integers with a size of \( M \times N \). Each element is either 0 or 1, representing the binary matrix. The data type is `int`.

Sub-Components:
- None