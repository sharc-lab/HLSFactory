open_project project_1

# Add Vitis include paths
#config_compile -include_path "/tools/software/xilinx/Vitis/2024.2/include"
#config_compile -include_path "/tools/software/xilinx/Vitis/2024.2/vcxx/data/include"

# set top function of the HLS design
set_top circular_convolution_3d

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
set_part {xave1752-vsva1596-1LJ-i-S}

# default frequency is 100 MHz
#create_clock -period 4 -name default

# C synthesis for HLS design, generating RTL
csynth_design

exit