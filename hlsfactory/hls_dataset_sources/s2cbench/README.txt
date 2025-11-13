===================================================================================
				S2CBENCH v.2.2
===================================================================================

S2CBENCH stands for Synthesizable SystemC Benchmark suite. It is a open source
SystemC benchmarks created to help designers evaluate the QoR of state of the art
HLS Tools. All of the main HLS tools support the synthesizable subset of SystemC
and hence should be able to synthesize all of these designs without any modifications.

Most of the work has been done by the DARClab at the Department of Electrical andComputer Engineering (ECE), at the  University of Texas at Dallas. 
Previous versions were done by the same lab at PolyU in Hong Kong.
DARClab=Design Automation and Reconfigurable Computing Laboratory - www.utdallas.edu/~schaferb/darclab

More details regarding the benchmarks can be found at the following academic publications:

B. Carrion Schafer and A. Mahapatra,"S2CBench:Synthesizable SystemC Benchmark Suite for High-Level Synthesis",
IEEE Embedded Systems Letters, Vol. 6(3), pp. 53-56, 2014

Please cite this work if you use S2CBench for your academic work.

S2CBench is distributed in the hope that it will be useful. S2CBench is free software; you can 
redistribute it and/or modify it, but please remember WITHOUT ANY WARRANTY; without even the 
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  

S2CBENCH includes the following Testcases:

------------------------------------------------------------------------------------
|   NAME      |        Description                      |     Author   
|-------------+-----------------------------------------+---------------------------
| adpcm       | Adaptive Differential Pulse-Code        | FFmpeg
|             | Modulation (encoder part only)          | PolyU DARClab 
|-------------+-----------------------------------------+--------------------------
| ann         | Artificial Neuronal Network (ANN)       | David Aledo, CEI, ETSII, Universidad Politecnica Madrid 
|             | 2 and 4 layer version                   | PolyU DARClab 
|-------------+-----------------------------------------+--------------------------
| aes         | Advanced Encryption standared (AES)     | pjc.co.jp 
|             | 128-bits (cipher and inv cipher)        | Shuangnan Liu, PolyU DARClab 
|aes_cipher   | Only aes cipher part                    |
|aes_invcipher| Only aes inv cipher  part               |
|-------------|-----------------------------------------|---------------------------
| ave8        | moving average of last 8 numbers read   | UTD DARClab
|             |                                         |
|-------------+-----------------------------------------+--------------------------
| cholesky    |  Cholesky demcomposition                | UTD DARClab
|             |                                         |  
|-------------+-----------------------------------------+--------------------------
| disparity   | Stereoscopic image disparity estimator  | Shuangnan Liu
| estimator   |                                         | PolyU,UTD DARClab 
|-------------+-----------------------------------------+--------------------------
| fft         | Fast Fourier Transform                  | R.Goswami,Synopsys, Inc.
|             | (fixed point)                           | PolyU,UTD DARClab 
|-------------+-----------------------------------------+---------------------------
| fir         | 9-Tap FIR filter                        | PolyU,UTD DARClab 
|-------------+-----------------------------------------+--------------------------
| decimation  | 5 Stages decimation filter              | PolyU DARClab
|-------------+-----------------------------------------+--------------------------
|interpolation| 4 Stages interpolation filter           | PolyU,UTD DARClab
|-------------+-----------------------------------------+--------------------------
| JPEG        | JPEG encoder                            | Jianqi Chen, UTD
| encoder     |                                         | DARClab
|-------------+-----------------------------------------+--------------------------
| JPEG        | JPEG decoder                            | Shuangnan Liu, PolyU
| decoder     |                                         | DARClab
|-------------+-----------------------------------------+--------------------------
| idct        | Inverse Discrete Cosine Transform       | Thomas G. Lange
|             |                                         | PolyU DARClab
|-------------+-----------------------------------------+--------------------------
| kasumi      | Kasumi encryption algorithm             |  ETSI/SAGE
|             |                                         |  PolyU DARClab
|-------------+-----------------------------------------+--------------------------
| md5c        | Message Digest Algorithm                | RSA Data Security, Inc
|             |                                         | PolyU DARClab
|-------------+-----------------------------------------+--------------------------
| qsort       | Quick sort                              | Darel Rex Finley 
|             |                                         | PolyU DARClab
|-------------+-----------------------------------------+--------------------------
| uart        | Universal Asynchronous                  | Nandeesh Veeranna,PolyU DARClab
|             | Receiver/Trasmitter                     | 
|-------------+-----------------------------------------+--------------------------
| snow3G      | snow 3G encryption algorithm            | ETSI/SAGE
|       |     | PolyU DARClab
|-------------+-----------------------------------------+--------------------------
| sobel       | Sobel filter                            | Anushree Mahapatra, PolyU DARClab
|             |                                         |
|-------------+-----------------------------------------+------------------------
| VGA         | VGA controller and image generator      | Siyuan Xu, PolyU DARClab  
|             |                                         |   
|---------------------------------------------------------------------------------


Each benchmark contains the following files:

Makefile -- Need to modify the path to the systemC folder
---------
make : Generates the executable binary
make wave :Generates the same binary, but the simulate creates a VCD file to view the simulation results
make debug : to create a debug version	   -- VCD File can be visualied with GTKwave (free VCD file viewer)
make clean : cleans the exe and .o files

SystemC files
------------
main.cpp  : Instantiates the modules which sends and receives data and the unit under test
benchmark.cpp /.h : Main description of the benchmark
tb_benchmark.cpp /.h : Testbech for the given benchmark
define.h  : Includes define statments and stimuli filenames

Stimuli files (.txt)
-------------------
<name>.txt       :  File with iput stimuli (could be more than one)
<name>_golden.txt :  File with golden output with which the simulation results will be compared


Extraction instruction (Linux):
%tar -zxvf S2Cbench_<ver>.tar.gz


Revisions
Version 2.2  update notes:
-------------------------

Design updates:


New designs added to the benchmark suite:
1.- new aes cipher

Misc changes:
1. redesigned the disparity estimator
2. add shift registers in FIR filter
3. changed coefficients and I/O configuration of interpolation
4. fixed sum overflow in ave8
5. modified the testbench for sobel, enabled HD mode
6. eliminated extra EOB(end of block) generated by RLE in JPEG Encoder when last quantized element is non-zero
 




							[END]





