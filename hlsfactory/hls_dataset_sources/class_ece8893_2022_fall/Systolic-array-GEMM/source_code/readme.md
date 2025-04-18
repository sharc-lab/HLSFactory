# Acceleration of Matrix Multiplication using Systolic Arrays


An accelerator for matrix multiplication using 16x16 2-D systolic array. 

- Matrix multiplication is done using output stationary tiling method. 
- The processing elements in the systolic array are connected through streams and dataflow is used to parallelize the computation, read/write operations of the processing elements.

-------------------------


## Source Files 

- Source.cpp : contains matrix_mat function, which is the top module of the accelerator.
- TestBench.cpp : contains the testbench code for verifying the functionality.
- dcl.h : Matrix dimensions and tile size are defined.
 -------------------------

## How to run the code
- Create a project in Vitis HLS and add the Source.cpp and dcl.h files to the source folder.
- Add TestBench.cpp file to the Test bench folder.

#### Step 1: C simulation
Just type make and ./result to test if your Vitis HLS function is correct.
Important: please constantly run C simulation after every change you have made to matrix_mat function.

#### Step 2: C synthesis
Use the GUI to run the C synthesis. 
After synthesis, you can either open the GUI to read the reports, or you can find the reports under ./project_1/solution_1/syn/report folder. The reports ending with .rpt can be opened using text editors.
-------------------------

## Points to Note
- Do not change the TILE size present in dcl.h.
-  The code works only for square matrices (M=N=K)
- The matrix dimension can be changed in dcl.h (M, N and K)


