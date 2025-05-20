open_project neural_project

# set top function of the HLS design
set_top last_layer_kernel

# add source file
add_files top.cpp

# add testbench
add_files -tb host.cpp

# add data file
add_files -tb input_1.bin
add_files -tb last_layer_weights_clean.bin
add_files -tb output_1.bin

open_solution "solution1"

# FPGA part and clock configuration
set_part {xcvu19p-fsva3824-2-e}

# default frequency is 100 MHz
#create_clock -period 4 -name default

# C synthesis for HLS design, generating RTL
csynth_design

exit
