open_project shibao_su_lab_3_proj

# set top function of the HLS design
set_top sparse_matrix_multiply_HLS

# add source file
add_files top.cpp

# add testbench
add_files -tb host.cpp

# add data file
add_files -tb A_matrix_csr_sparsity_0.10.bin
add_files -tb B_matrix_csc_sparsity_0.10.bin
add_files -tb C_matrix_result_sparsity_0.10.bin
add_files -tb A_matrix_csr_sparsity_0.50.bin
add_files -tb B_matrix_csc_sparsity_0.50.bin
add_files -tb C_matrix_result_sparsity_0.50.bin
add_files -tb A_matrix_csr_sparsity_0.80.bin
add_files -tb B_matrix_csc_sparsity_0.80.bin
add_files -tb C_matrix_result_sparsity_0.80.bin

open_solution "solution1"

# FPGA part and clock configuration
set_part {xczu3eg-sbva484-1-e}

# default frequency is 100 MHz
#create_clock -period 4 -name clk

# C synthesis for HLS design, generating RTL
csim_design

csynth_design

# C/RTL co-simulation; can be commented if not needed
#cosim_design

# export generated RTL as an IP; can be commented if not needed
#export_design -format ip_catalog -flow impl



exit