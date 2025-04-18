# ECE 8893 FPG Final Project: Accelerating the Attention Layer of the Transformer Model using an Optimized Dataflow

## Golden Binary
The golden inputs and outputs are too large to maintain in our github repo without using Git Large File Storage, so we are maintaining these separately.
Please reach out to authors for more information.

## C++ implementation
Our C++ base code can be found in `old_flat.cpp`, and can be compiled and run with the Makefile in the base directory.
`old_flat.cpp` contains the functions for computing logit, softmax, and attention for the entire 4D tensor.  `utils.cpp` contains the code for loading key, value, query, and bias inputs, and writing output, but has since been modified and optimized for the HLS implementation. The user can select whether to run floating or fixed point simulation in `config.h` (though floating point simulation doesn't currently work).

This code can be compiled and run with `make all && ./csim.out`.  NOTE: you may need to run `ulimit -s unlimited` to prevent the program from running out of stack space.

## HLS Implementation
Our HLS implementation resides in `flat.cpp`, `systolic_array.cpp`, and `utils.cpp`.  `flat.cpp` contains the top level function for optimization. `systolic_array.cpp` contains the code for our two systolic arrays, as well as the optimized softmax function.  `utils.cpp` contains functions for reading inputs to and writing outputs from DRAM.

This code can be be run and generate reports with `vitis_hls -f run_hls.tcl`.  The current code is not the highest performance code we have come up, which is recorded in the report, as we have continued trying different schemes.

## References

FLAT Paper: S.-C. Kao, S. Subramanian, G. Agrawal, and T. Krishna, “Flat: An optimized dataflow for mitigating attention performance bottlenecks,” 2021. https://arxiv.org/abs/2107.06419

Existing systolic array implementation:

* Johannes de Fine Licht, Grzegorz Kwasniewski, and Torsten Hoefler. "Flexible Communication Avoiding Matrix Multiplication on FPGA with High-Level Synthesis." In Proceedings of the 2020 ACM/SIGDA International Symposium on Field-Programmable Gate Arrays (FPGA'20). https://github.com/spcl/gemm_hls

* https://github.com/Xilinx/Vitis_Accel_Examples/tree/master/cpp_kernels/systolic_array
