

# Special-Topics 8893

# Edge Detection Algorithm Acceleration in FPGA 
1. What algorithm ? Canny and Sobel Edge detection 
2. What kind of Memory access ? DMA (configure with Zynq processor on control register with starting address,no of bursts, lenght) 
3. Image Input >> UART from Ext DDR
4. What is mode of sending Image ? Burst mode 
5. Which FPGA board ? Ultra96/Pynq/ZCU102
6. Interface testing ? AXI protocal interface
7. IP integration and testing ? Intergrated Logic Analyzer IP

# Motivation

Classification of Brain tumour is very helpful for the radiologist to affirm their diagnosis.

![Alt Text](https://github.com/rithanathith/Special-Topics---8893--Project-/blob/branch_1_RA/pics/brain_detect.png)

WHO estimates the more than 4 lakh children are suffering from tumour every year.Early detection of tumour lead to better chance of survival
Detection of brain tumour MRI images.

# What makes good detector ?
 1. Good detection - able to detect as many edges as possible
 2. Good Localization  - able to match the exact location of edges with original images
 3. Response time - detect true edges (scenario of edges detected twice, noice can create false edges)

## Latency

> Sobel algorithm - simple & takes less time.

> Canny algorithm - complex and provides sharp edges

## Performance wise

Canny > Sobel

Canny gives good performance in the noisy images.

Computationally intensive >> more power and delay 
for matching the real time constraints.

 Our goal is to optimize the computationally intensive canny algorithm and prune out the unwanted edges from the MRI image.


## Noise removal 

Conver the RGB to grayscale image 

Step 1 : Guassian Filter - Smoothening the image by bluring .
We subtract the original image with o/p of guassian filtered Image. This is called Unsharp Masking.

> * Blur the image by guassian low pass filter

***

Step 2 : Compute Guassian value G, Guassian direction (edge angle).

The conventional Sobel contains horizontal and vertical convolution, and the Sobel operator uses two matrices to convolve the original image to calculate an estimate of the gray difference (partial
derivative) in two directions (horizontal and vertical).

> Gx and Gy

Gx = Image * Horizontal_ Kernal 

Gy = Image * Vertical_ Kernal 

> * Sober Filtering and compute magnitute and phase 

G = root (Gx ^2 + Gy^2) = mod(Gx) + mod(Gy)

phase fi = 90 degree , mod(Gx) < mod(Gy)

phase fi = 0 degree , mod(Gx) > mod(Gy)

***

***
Above method of detection has the problem of low edge detection accuracy.

For example, the liver and other objects with closed contour still have the loss of edges in some directions.
***

Solution is to increase the no. of kernals convolved with image in various direction. But this will increase the memory access latency and on chip power. Also less efficient in noisly image.

So we need to formulate some technique which can take care of suppressing the unwanted edges and make it sharper. >> CANNY ALGORITHM

***

Step 3 : Non max suppresion - Edge Thinning 
local maxima is location where the change in intensity is very keen.

>Convert the blurred egdes to sharp edges
>preserve all the local maxima in the gradient image and delete everythign else

1. Round the gradient direction to nearest 45◦, corresponding to the use of an 8-connected neighborhood.
2. Compare the edge strength of the current pixel with the edge strength of the pixel in the positive and negative gradient direction. I.e. if the gradient direction is north (theta = 90◦), compare with the pixels to the north and south.
3. If the edge strength of the current pixel is largest; preserve the value of the edge strength. If not, suppress (i.e. remove) the value.

***

Step 4: Double Hysteresis Threshold -  to decrease incorrect edge points and detect the true edges.

** Set the high and low threshold

> T high  and
> T low

*  T high saturates every pixel with a gradient value greater than its threshold value. 
*  T low bypasses every pixel with a gradient value greater than its threshold value.
  
*  The output of these two blocks are added up, resulting in a stream where the saturated pixels are considered part of the edges and the other pixels different than zero are considered edge candidates.


## Memory Access 

DMA - Direct memory access - AXI lite protocal which is address mapped protocal.

PL - Canny IP is axi stream protocal for getting burst of data with no address.

Bus arbitrater decides when DMA or processor has to act like master on the system bus 

UART - Host PC and BOARD


## Analyze 

* Mean Square Error (MSE)
* Root Mean Square Error(RMSE)
* Entropy
* Signal to Noise Ration(SNR) 
* Peak Signal to Noise Ratio(PSNR)


• A thorough comparison between both solutions in terms of resource usage, execution time, and programming effort. In this way, we can identify the strengths and weaknesses of each programming approach in the image processing field.

# IEEE Content for Week 1 Submission 

## Abstract
#########################################################
## Introduction : 
1. General Overview & your proposed approach 

## Problem Description: 
1. Discrepency betweeen HDL and HLS on FPGA
2. Canny Algorithm
3. Importance of canny 
4. Why FPGA ?
5. Why Image processing in FPGA ?

## Related Work : State of Art, canny algo

1. Literature Survey of HLS and HDL
2. Canny algorithms explaination Stage wise 

## Proposed Approach 

1. Canny algorithm how we are planning to implement in FPGA 
2. Implementation stategies on FPGA
3. Accerating with pragmas and other behavioural techniques 
3. Verilog analysis and comparision between software and hardware model


## Final Code
# In HDL folder, 

1. hdl contains the design_wrapper.bit file
2. srcs contains verilog files required for canny computation 
3. hw_handoff contains the handoff file for pynqz2
4. sim contains the testbench for all individual block 
5. sim_block contain the design.v 
6. synth contains the synthesized netlist
7. report contains the synthesis and implementation report from Vivado HDL


# In HLS folder, 

1. board_test contains the bitstream file, hardware handoff file and python code to test on-board
2. python_lut contains python files to store data for tan function using LUTs
3. hls contains the files required for implementing canny algorithm
4. utils contains the files to read data from bitmap image to array and write data from array to bitmap image
5. sim contains the testbench file for testing the canny algorithm
6. Makefile can be used to compile and run the c simulation and synthesis using terminal
7. script.tcl is used to run synthesis 
8. csynth.rpt contains the synthesis report

# In Golden C Code folder,

1. contains files used as a reference to generate HLS code for our implementation of Canny algorithm.

# In demo video 

1. contains the on-board demo video 


# In pics 
1. contains all pics of input and output results and computation required.


# References

{1} Z. Tan and J. S. Smith, “Real-time canny edge detection on fpgas using high-level synthesis,” 2020 7th International Conference on Information Science and Control Engineering (ICISCE), 2020.


{2} B. Li, J. Chen, X. Zhang, X. Xu, Y. Wei, and D. Kong, “A design of Zynq-based medical image edge detection accelerator,” 2021 6th International Conference on Biomedical Signal and Image Processing, 2021. 


{3} R. Dobai and L. Sekanina, “Image filter evolution on the Xilinx Zynq platform,” 2013 NASA/ESA Conference on Adaptive Hardware and Systems (AHS-2013), 2013.  


{4} R. Millón, E. Frati, and E. Rucci, “A comparative study between HLS and HDL on SOC for image processing applications,” Elektron, vol. 4, no. 2, pp. 100–106, 2020.

{5} H. M. Abdelgawad, M. Safar, and A. M. Wahba, “High Level Synthesis of Canny Edge Detection Algorithm on Zynq Platform,” World Academy of Science, Engineering and Technology International Journal of Computer and Information Engineering, vol. 9, no. 1, 2015. 


{6} M. Gurel, “A comparative study between RTL and HLS for image processing applications with fpgas,” thesis. 


{7} D. Daru, “Implementation of canny edge detection using hls(high level synthesis),” thesis. 


{8} K. Yoshikawa, N. Iwanaga, T. Hamachi, and A. Yamawaki, “Development of fixed-point canny edge filter operations for high-level synthesis,” The Proceedings of the 2nd International Conference on Industrial Application Engineering 2015, 2015. 