# ECE 8893 - ACO & Local search Optimization Project

## FACO 

This project was based off the Focused Ant Colony Optimization algorithm, which was an algorithm for solving large TSP instances as described in the paper:

    R. Skinderowicz,
    Improving Ant Colony Optimization efficiency for solving large TSP instances,
    Applied Soft Computing, 2022, 108653, ISSN 1568-4946,
    https://doi.org/10.1016/j.asoc.2022.108653

The original code for FACO can be found at https://github.com/RSkinderowicz/FocusedACO. Executing the original code requires a computer with C++17 and with OpenMP.

### Compilation and Execution
The code in the FACO folder has been modified to remove all OpenMP pragmas (in order to run on the ece-linlabsrv01 server) and to add code to dump inputs and outputs for local search into a text file. It can be compiled with:

    make

and can be run by:

    ./faco -p instances/<some tsp file>
    
which will dump inputs and outputs of the first 20 iterations of local search to the "stats.txt" file.
    
## 2-opt Local Search Unoptimized - HLS/LocalSearchUnoptimized

### Contents
This folder contains the code used to run tests on the local search. This has not been optimized using FPGA pragmas yet. However significant changes have been made to adapt the LS code from FACO to work on an FPGA. Using the stats.txt which contains data dumped for 20 iteratiosn including the inputs and outputs of LS from FACO, a test bench is created. This is in main.cpp. 

two_opt_nn.cpp includes the top function (two_opt_nn) running on the FPGA board, and a couple of helper functions including the subroutine that flips the order of the routs (flip route section).

The folder also includes the .bit and .hwh files required for on-board testing. Some of the results are also stored in a directory called ResultFromReports. 

### Compilation and Execution
Compilation of the Unoptimized 2-opt LS code can simply done by navigating to the folder and can be done by executing

    make
   
To run the code, execute:

    ./result
    
To synthesize (Synthesis runs the tcl script which can be modified to run csim, run cosim, csynth, or even export the ip for vivado): 

    make synth
    
The ece-linlabsrv01 server was used to compile, run, and synthesize this code.

## 2-opt Local Search Optimized - HLS/LocalSearchOptimized

### Contents
This folder contains code with optimized HLS code implementing 2-opt local search, which is in the file "two_opt_nn.cpp," along with testing files.

two_opt_nn.cpp includes the top function (two_opt_nn) running on the FPGA board, and a couple of helper functions including the subroutine that flips the order of the routs (flip route section).

Unlike the unoptimized code, the optimized HLS code in this directory does not include the bitstream and hardware handoff files generated from Vivado. This is left to the user to regenerate. This directory also does not include the Jupyter notebook to run the optimized code on-board. This notebook resembles the same structure as the notebook for running the unoptimized code on-board.

### Compilation and Execution
Compilation of the Unoptimized 2-opt LS code can simply done by navigating to the folder and can be done by executing

    make
   
To run the code, execute:

    ./result
    
To synthesize (Synthesis runs the tcl script which can be modified to run csim, run cosim, csynth, or even export the ip for vivado): 

    make synth
    
The ece-linlabsrv01 server was used to compile, run, and synthesize this code.

## Population-based Ant Colony Optimization
This folder contains the "paco.cpp" file, which attempts to implement Population-based Ant Colony Optimization, as described in this paper:

    M. Guntsch et al., 
    "Population based ant colony optimization on FPGA," 
    2002 IEEE International Conference on Field-Programmable Technology, 2002. (FPT). Proceedings., 
    Hong Kong, China, 2002, pp. 125-132, 
    doi: 10.1109/FPT.2002.1188673.
    
PACO involves using a population matrix of the best routes of the last k rounds to update edge weights when selecting new routes. As it did not provide good results, it was shelved and not tested with HLS.

### Compilation and Execution
To compile, navigate to the PACO folder and execute

    g++ paco.cpp ../FACO/src/problem_instance.cpp -o paco
    
and to run, execute

    ./paco <some tsp file>
    
Note that examples for tsp files to run with PACO are also in the PACO folder. PACO does not work with problem instances that are the size used with FACO.

No special environment settings are needed; any standard environment with C++ will work.

## 3-opt Local Search (Unoptimized) - HLS/three-opt
We also attempted to potentially acclerate 3-opt local search. As an implementation that used the same nearest neighbors data structure that 2-opt LS used was not there, the first critical step was to convert the provided, naive implementation of 3-opt LS into a version that used nearest neighbors information in order to reduce time complexity.

"three_opt_nn.cpp" contains code for 3-opt local search that has been adjusted to use nearest neighbor information and to be synthesizable. The results of this implementation of 3-opt LS were worse than that of 2-opt LS, and so this implementation was not optimized for the purposes of HLS.

### Compilation and Execution
Compilation of the 3-opt LS code can simply done by navigating to the folder and can be done by executing

    make
   
To run the code, execute:

    ./result
    
To synthesize:

    make synth
    
The ece-linlabsrv01 server was used to compile, run, and synthesize this code.
