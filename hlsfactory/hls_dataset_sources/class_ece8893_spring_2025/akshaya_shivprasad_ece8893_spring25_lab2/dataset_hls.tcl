open_project akshaya_shivprasad_ece8893_spring25_lab2_proj

# add source files
add_files top.cpp
add_files -tb host.cpp
add_files -tb Q_tensor.bin
add_files -tb V_tensor.bin
add_files -tb K_tensor.bin
add_files -tb Output_tensor.bin

open_solution "solution1"

# set top function of the HLS design
set_top compute_attention_HLS

# FPGA part and clock configuration
set_part {xczu3eg-sbva484-1-e}

# default frequency is 100 MHz
create_clock -period 2 -name clk

# C synthesis for HLS design, generating RTL
csynth_design

exit