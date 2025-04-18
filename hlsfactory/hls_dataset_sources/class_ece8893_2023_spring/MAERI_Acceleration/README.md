# ECE8893 FPGA Final Project -- Using HLS to accelerate MAERI

## Implementations included

1. Arbitrary IFC-Buffer Burst Transfer

2. ReLU

3. Batch Normalization

4. Max Pooling 

5. RAFFT network 

## How to run the code?

Please use the vitis_hls GUI to create a new project and add the maeri_v2_1.cpp and maeri_v2_1.h files in the same source folder.

Also, add maeri_v2_1_tb.cpp file in the TestBench folder to run C simulation.

To run the synthesis, use vitis_hls GUI to generate the corresponding synthesis report

For more details about how to build up the vitis_hls project, please refer to Akshay's HLS tutorial.

The link to Akshay's tutoral: https://akshaykamath.notion.site/HLS-Tutorial-dc7e388dc31641fba5002012e3e69204

## More about the code

At the top of the maeri_v2_1.cpp file, there are several defined macros that help to run different testings. The default setup utilizes the DSP for Batch Normalization. 

Comment the BN_MULT_OP_DSP macro and uncommet the BN_MULT_OP* macros to enable differnt multipliers.

If you want individual testings for the functions, you can also comment out the corresponding macros. 

Copyright: Pls contact authors for copyright before publicly release. (email: jianming.tong@gatech.edu, aitagi7@gatech.edu, geonhwa.jeong@gatech.edu, hwu419@gatech.edu) 