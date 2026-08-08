Kernel Description:
The `CumulativeSums` kernel is designed to compute the cumulative maximum and minimum values of a sequence of integers derived from a binary input array `epsilon`. The algorithm iterates through the `epsilon` array, updating a cumulative sum `S` based on the values of `epsilon`. If `epsilon[k]` is 1, `S` is incremented; if `epsilon[k]` is 0, `S` is decremented. The kernel tracks the maximum value (`sup`) and minimum value (`inf`) that `S` attains during the iteration. The results are stored in the output pointers `res_sup` and `res_inf`, respectively. The kernel is optimized for high-level synthesis, focusing on efficient dataflow and minimal resource usage.

---

Top-Level Function: `CumulativeSums`

Complete Function Signature of the Top-Level Function:
`void CumulativeSums(int *res_sup, int *res_inf, int epsilon[N]);`

Inputs:
- `res_sup`: A pointer to an integer where the maximum cumulative sum will be stored. The data type is `int`.
- `res_inf`: A pointer to an integer where the minimum cumulative sum will be stored. The data type is `int`.
- `epsilon`: An array of integers of size `N` (20000) containing binary values (0s and 1s). The data type is `int[N]`.

Outputs:
- `res_sup`: The maximum cumulative sum value is stored at the memory location pointed to by `res_sup`.
- `res_inf`: The minimum cumulative sum value is stored at the memory location pointed to by `res_inf`.

Important Data Structures and Data Types:
- `epsilon`: An array of integers of size `N` (20000) containing binary values (0s and 1s). The data type is `int[N]`.

Sub-Components:
- None