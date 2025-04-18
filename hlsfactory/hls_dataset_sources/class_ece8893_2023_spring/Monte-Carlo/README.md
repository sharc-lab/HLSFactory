# ECE-8893-Monte-Carlo-HLS

This project performed design exploration to Generate Random Numbers and Geometric Brownian Motion Paths.

The Directories are developed as follows:

Paths: Generates a certain number of paths based on the number of Paths that is updated in optionValueAverage.hpp's variable: NUM_PATHS
NUM_STEPS is how many time steps are in each variable.
N_BUFF is the number of paths that are written into BRAM. For 100 paths, set to 100. On the Pynq-Z2, N_BUFF was found to have a maximum capacity at 500.

The demo files demo1000.csv and demo100.csv are normal random distributions generated from an np.random.normal distribution.


MersenneTwister Generates a random normal distribution set of 1,000 numbers. 

Random 100 Paths Generates a random distribution of 100 time steps with 100 paths. 

Each directory has a Makefile and a tcl script that allows for users to compile the code and generate the synthesis files. 


This project utilized the following links for reference:


[1] V. Cvetanoska and T. Stojanovski, Using high performance computing and Monte Carlo simulation for pricing american options,
arXiv:1205.0106 [cs], May, 2012.

[2] F. Y. Wang, Distributed Monte Carlo Simulation for Option Pricing:
The first completed benchmark and applications of distributed Monte
Carlo simulation model on high-performance computing architecture,
2009 International Conference on Cyber-Enabled Distributed Computing
and Knowledge Discovery, 2009.

[3] Matsumoto, M., & Nishimura, T. (1998). Mersenne Twister: A 623-
Dimensionally Equidistributed Uniform Pseudo-Random Number Generator. ACM Transactions on Modeling and Computer Simulation, 8(1),
3-30.