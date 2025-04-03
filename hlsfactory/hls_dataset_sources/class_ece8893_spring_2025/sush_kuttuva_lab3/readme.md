# Lab 3: Efficient Implementation of Sparse Matrix Multiplication

## Objective

Sparse matrix multiplication is a critical operation in scientific computing, machine learning, and graph-based algorithms. Optimizing it for FPGAs involves managing irregular memory access patterns, handling sparse data efficiently, and balancing hardware resources.

In this lab, we will:

1. Implement the sparse matrix multiplication $C = A \times B$, where $A$ is in CSR format, $B$ is in CSC format, and $C$ is a dense matrix.
2. Load matrices $A$ and $B$ from binary files, compute the result $C$, and store the output $C$ as a binary file.
3. Optimize your implementation to minimize latency and ensure it fits within FPGA resource constraints.

---

## Problem Statement

Your task is to implement the **sparse matrix multiplication** operation:

$$
C[i][j] = \sum_k A[i][k] \cdot B[k][j]
$$

### Inputs:

- Sparse matrix $A$ in CSR format:
  - `values_A[]`, `column_indices_A[]`, `row_ptr_A[]`.
- Sparse matrix $B$ in CSC format:
  - `values_B[]`, `row_indices_B[]`, `col_ptr_B[]`.
- Both matrices $A$ and $B$ will be loaded from binary files:
  - `A_matrix_csr.bin`
  - `B_matrix_csc.bin`.
- Multiple sets of testbenches are given under different sparsity ratios: 0.1, 0.5, and 0.8.

### Outputs:

- Dense matrix $C$ stored in a binary file `C_matrix_result.bin`.

### Reference Implementation:

- A reference implementation (unoptimized) is provided as a baseline. Your goal is to optimize this implementation for latency while ensuring correctness and hardware implementability.

---

## Design Constraints

### Must:

1. **No DRAM Partitioning**:
   - Avoid partitioning arrays in DRAM.
2. **Place-and-Route Success**:
   - Your design must be implementable and generate a valid bitstream.
3. **Correctness**:
   - Ensure that your computed $C$ matches the reference implementation within a small tolerance.
4. **Report Accurate Latency**:
   - Report latency using either **C/RTL co-simulation** or **LightningSim**. Using [**LightningSim**](https://github.com/sharc-lab/LightningSim) is highly encouraged for quicker iterations.

---

## Deliverables

1. **Source Code**:
   - Provide well-documented HLS source code (`.cpp` and `.h` files).
   - Provide HLS synthesis report, C/RTL co-simulation report (or LightningSim screenshot), and post-implementation resource utilization report.
2. **Bitstream File**:
   - Submit the generated `.bit` file to confirm your design is implementable.
3. **Technical Report**:
   - Include the following in your report:
     - Key optimizations applied.
     - Resource utilization (LUTs, BRAMs, DSPs, FFs).
     - Identified bottlenecks and how they were resolved.
     - Post-implementation latency.
     - Comparison between computed and reference outputs.
     - Anything else worth discussing.

---

## Evaluation Criteria

### 1. Implementation Score (80%)

Your implementation will be scored as follows:

- **Correctness** (a): a = 1 - MSE

- **Implementability** (b): b = 1 if the design can place-and-route and generate a valid bitstream; b = 0 otherwise.

- **Speedup Ratio** (s_i): s_i = baseline_latency_sparsity_i / your_latency_sparsity_i
    - Your latency is compared against the unoptimized baseline latency, under sparsity ratios of 0.1, 0.5, 0.8.

- **Relative Speedup Score** (r): Competes within the class:
    - If speedup is smaller than 10, r_i = 0.1;
    - othersies, r_i = 0.2 + 0.2 * (1 - (your_rank_sparsity_i - 1} / (total_submission_sparsity_i - 1))
    - r = sum(r_i), i = 0.1, 0.5, 0.8

- **Final Implementation Score**: f = 80 * r * b * a

---

### 2. Report Score (20%)

Your report will be manually graded based on:

- Explanation of optimizations.
- Bottleneck identification and resolution.
- Clear presentation of results and comparisons.

Optional: you can try to comprehensively analyze: 
- The performance benefit and overhead of using sparse matrix representation and multiplication under different sparse ratios.
- When is it suitable to use dense matmul, and when is it suitable to use sparse matmul?
- What if matrices A and B use the same sparse format?

---

## Submission

Submit the following files:

1. **Source Code**: `.cpp` and `.h` files.
2. **Bitstream File**: `.bit` file.
3. **Report**: `.pdf` file.

---

## Hints and Tips

1. **Start Simple**:
   - Implement a naive version of sparse matrix multiplication first.
   - Validate correctness with small matrices (e.g., \( N = 8, M = 8, K = 8 \)).

2. **Incremental Optimization**:
   - Gradually apply pipelining, dataflow, and unrolling to improve latency.

3. **Analyze Memory Access**:
   - Optimize memory access patterns to minimize idle cycles during computation.

4. **Utilize LightningSim**:
   - Use LightningSim for quick latency estimation and to explore optimization strategies.

---

Good luck with your implementation!
