open_project proj

# this is your top module 
set_top resnet18

# this is your file that to be synthesized as FPGA design
add_files resnet18.cpp
add_files conv1/conv1.cpp 
add_files conv_ds/conv_ds.cpp
add_files conv_3x3_s1/conv_3x3_s1.cpp
add_files avg_pool/avg_pool.cpp
add_files linear_fc/linear_fc.cpp

# this is your testbench (specified by -tb option)
# add_files -tb sim.cpp

open_solution "solution1" -flow_target vitis

# Set board to Pynq-Z2
set_part {xc7z020clg400-1}

create_clock -period 10 -name default
config_interface -m_axi_latency 64
config_interface -m_axi_max_widen_bitwidth 256

# synthesis
csynth_design

# Run co-simulation
#cosim_design

# Export IP to Vivado
#export_design -format ip_catalog

exit
