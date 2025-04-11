# Lab 2: Efficient Implementation of Attention Computation

## Objective

The attention mechanism is a core component of Transformers and Large Language Models (LLMs) that drives their power and scalability. By computing a weighted combination of input features, attention enables efficient parallelization, context-aware representations, and versatility.

In this lab, we will:

1. Implement the `compute_attention` function for a scaled dot-product attention mechanism in HLS.
2. Load input tensors from binary files and compare your computed output tensor with the provided reference output.
3. Optimize your implementation to minimize latency while staying within resource constraints.

---

## Problem Statement

Your task is to implement the `compute_attention` function, which performs scaled dot-product attention:

$$
\text{Attention}(i, j) = \text{Softmax}\left(\frac{Q \cdot K^\top}{\sqrt{d_k}}\right) \cdot V
$$

### Inputs:

- Tensors (`Q`, `K`, `V`) provided as binary files:
  - `Q_tensor.bin`
  - `K_tensor.bin`
  - `V_tensor.bin`

### Outputs:

- Compute the output tensor (`Output_HLS`).
- Compare it against the provided reference tensor (`Output_tensor.bin`).
- Compute the Mean Squared Error (MSE).

### Reference Implementation:

A reference implementation (unoptimized) is provided as a baseline. Your goal is to optimize this implementation for latency while ensuring correctness and hardware implementability.

---

## Design Constraints

### Requirements:

1. **No DRAM Partitioning**:
   - Avoid partitioning arrays in DRAM.
2. **Place-and-Route Success**:
   - Your design must be implementable, i.e., can finish place and route.
3. **Fixed Dimensions**:
   - Do not change `B`, `N`, `dk`, or `dv`.
4. **Report Accurate Latency**:
   - Report latency using **C/RTL co-simulation** or **LightningSim** (recommended for faster results).

   > **Note**: Running co-simulation may take 1–2 hours. You may want to start with smaller matrices during the early stages of exploration.

---

### Tips and Notes:

1. **Data Loading**:
   - Enforce AXI burst mode and utilize full DRAM bandwidth.
   - Use efficient streaming or ping-pong buffering to load data into BRAM.
2. **Parallelization**:
   - Use **dataflow and streaming** to overlap data loading, computation (matrix multiplication, softmax), and writing results.
3. **Softmax Optimization**:
   - Simplify or approximate the softmax computation to reduce latency.
4. **Latency Analysis**:
   - Identify bottlenecks and optimize them (e.g., improve softmax efficiency or scaling operations).
5. **LightningSim**:
   - Use the LightningSim tool from Sharc Lab to quickly estimate total latency and compare results.
6. **Printing Values**:
   - When printing `ap_fixed` type values using `%f`, remember to add `.to_float()`; otherwise, it always prints zero.

---

## Deliverables

1. **Source Code**:
   - Provide well-documented HLS source code (`.cpp` and `.h` files).
   - Include the HLS synthesis report, C/RTL co-simulation report, and post-implementation resource utilization report.
2. **Technical Report**:
   - Include the following in your report:
     - Post-implementation resource utilization (LUTs, BRAMs, DSPs, FFs).
     - Post-implementation latency: `co-sim-cycle-count * post-implementation-clock-period`
     - Comparison between computed and reference outputs (to make sure your MSE is acceptable).
     - Key optimizations applied.
     - Identified bottlenecks and how they were resolved.
     - Anything else worth discussing.

---

## Evaluation Criteria

### 1. Implementation Score (80%)

Your implementation will be scored as follows:

- **Correctness** (a): a = 1 - MSE where MSE is the Mean Squared Error compared to the reference output. Small errors may occur if softmax approximations are applied.

- **Implementability** (b): b = 1 if the design can place-and-route (finish implementation); otherwise, b = 0.

- **Speedup Ratio** (s): s = baseline_latency / your_latency, where your latency is compared against the unoptimized baseline latency.

- **Relative Speedup Score** (r): Competes within the class:
if speedup ratio is smaller than 10, r = 0.2; otherwise, r = 0.6 + 0.4 x (1 - (your_rank - 1)/(total_submissions - 1)

- **Bonus** (e): e = max(0, 6 - your_rank)

- **Final Implementation Score**: f = min(80, r * b * a) + e

---

### 2. Report Score (20%)

Your report will be manually graded based on:

- Explanation of optimizations.
- Resource utilization analysis.
- Bottleneck identification and resolution.
- Clear presentation of results and comparisons.
- Use of IEEE double-column LaTeX format.

---

## Submission

Submit the following files:

1. **Source Code**: `.cpp` and `.h` files.
2. **Report**: `.pdf` file.

---

Good luck with your implementation!
