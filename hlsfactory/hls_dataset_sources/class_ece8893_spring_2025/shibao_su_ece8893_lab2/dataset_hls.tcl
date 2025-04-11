open_project shibao_su_lab_2_proj

# set top function of the HLS design
set_top compute_attention_HLS

# add source file
add_files top.cpp

# add testbench
add_files -tb host.cpp

# add data file
add_files -tb Q_tensor.bin
add_files -tb V_tensor.bin
add_files -tb K_tensor.bin
add_files -tb Output_tensor.bin

open_solution "solution1"

# FPGA part and clock configuration
set_part {xczu3eg-sbva484-1-e}
# default frequency is 100 MHz
#create_clock -period 4 -name clk

# default frequency is 100 MHz
#create_clock -period 4 -name default
csim_design
# C synthesis for HLS design, generating RTL
csynth_design

# C/RTL co-simulation; can be commented if not needed
#set ::AESL_AUTOSIM::gDebug 1
cosim_design

# export generated RTL as an IP; can be commented if not needed
#export_design -format ip_catalog -flow impl

exit