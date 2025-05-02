open_project symbolic_project

# set top function of the HLS design
set_top symbolic_circular_conv_HLS

# add source file
add_files top.cpp

# add testbench
add_files -tb host.cpp

# add data file
add_files -tb input1_A.bin
add_files -tb input1_B.bin
add_files -tb output1_C.bin

open_solution "solution1"

# FPGA part and clock configuration
set_part {xczu3eg-sbva484-1-e}

# default frequency is 100 MHz
#create_clock -period 4 -name default

# C synthesis for HLS design, generating RTL
csynth_design

exit
