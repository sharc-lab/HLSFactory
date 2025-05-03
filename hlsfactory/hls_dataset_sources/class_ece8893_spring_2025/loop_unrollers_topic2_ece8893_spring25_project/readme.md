# Final Project Topic 2: Connected Component Analysis

## Project Description
In this project, you will translate and optimize the given Python code, `build_track.py`, into High-Level Synthesis (HLS) to accelerate its functionality on an FPGA. This is a real problem -- HEP researchers are waiting for you.


The program processes input graphs stored in `.npz` files by performing the following steps:

1. **Graph Loading**: Reads graph data and edge probabilities from `.npz` files located in `/input_graphs/`.
2. **Connected Components Detection**: Identifies connected components in the graph using Depth-First Search (DFS) to determine disjoint tracks.
3. **Post-Processing**: Computes attributes for each track, including energy, momentum, particle ID, and others. These attributes are stored in NumPy arrays.
4. **Data Storage**: Writes the processed results back into `.npz` files.

Your task is to re-implement this functionality in HLS, ensuring efficient execution on FPGA hardware.

---

## Provided Resources
1. **Reference Code**: `build_track.py`
   - **Purpose**: Processes input graphs, identifies connected components, performs track-based analysis, and stores the results.
   - **Key Functionalities**:
     - `get_tracks`: Finds connected components using DFS.
     - `build_graph`: Processes graph edges and computes attributes for tracks.
     - `process`: Reads input files, processes graphs, and writes output.
2. **Input Data**: Located in `/input_graphs/`.
   - Contains `.npz` files with graph data such as edges, probabilities, and associated attributes.

---

## Key Implementation Notes
1. **Recursive to Non-Recursive DFS**: Convert the DFS algorithm into an iterative, non-recursive format, as recursion is not supported in hardware.
2. **Static Memory Usage**: Use statically sized arrays for all data structures to ensure compatibility with FPGA hardware, which does not support dynamic memory allocation.
3. **Library-Free Design**: Eliminate external libraries and re-implement required functionality using HLS-compatible constructs.
4. **Hardware-Friendly I/O**: Consider changing the input/output format to hardware-friendly alternatives, such as binary files, instead of dictionaries.
5. **Pipeline Optimization**: Process different input graphs in a pipelined manner to improve throughput and reduce latency.

---

## Evaluation Criteria
1. **End-to-End Latency**: Measure the total time required to process all input graph pairs, from reading input data to writing processed results back to DRAM.
2. **Post-Implementation Resource Utilization**: Report FPGA resource usage, including DSP slices, BRAM, LUTs, and FFs.
3. **Speedup Comparison**: Compare the latency of the FPGA implementation with the original Python implementation on a CPU. Report the speedup achieved.
4. **Ranking**: Projects will be ranked based on end-to-end latency and resource efficiency.

---

## Deliverables
1. **HLS Implementation**:
   - Submit well-commented HLS design files with detailed explanations of your design choices.
   - Use **HLSFactory** for submission unless publication is planned.

2. **Simulation Results**:
   - Verify correctness of your computation using simulation.

3. **Performance Report**:
   - **Latency**: Measure end-to-end latency for processing all input graph pairs using **C/RTL Co-Simulation** or **LightningSim**.
   - **Resource Utilization**: Provide detailed post-implementation resource usage data, including the generated bitstream.

4. **Technical Report**:
   - Use LaTeX in IEEE double-column format. Include:
     - **Design Overview**:
       - Algorithm-level details, including any optimizations made to improve performance.
       - Hardware component details, focusing on the core processing elements.
     - **Experimental Results**:
       - Performance metrics (latency, throughput, etc.) and resource usage.
       - Comparisons with baseline Python implementation on a CPU.
     - **Insights and Takeaways**:
       - Lessons learned, challenges encountered, and potential areas for improvement.

---

## Submission Instructions
- Submit all required files (code, simulation results, and reports) as a **zipped folder**.
- Ensure all files are clearly labeled and easy to navigate.

---

**Good luck with your project!**
