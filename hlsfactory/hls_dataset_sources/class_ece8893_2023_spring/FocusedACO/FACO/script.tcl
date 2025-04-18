open_project aco_ls_proj

# this is your top module (the function to be synthesized by HLS)
# TODO: this must be changed!!
set_top random_fpga_function

# these are your files to be synthesized as an FPGA design
add_files ./src/local_search.cpp
add_files ./src/utils.h

# this is your testbench (specified by -tb option)... any files not to be synthesized
add_files -cflags "-Isrc" -tb "./src/faco.cpp ./src/problem_instance.cpp ./src/progargs.cpp ./src/rand.cpp ./src/utils.cpp"

# TODO: flow_target might need to be changed to vitis for FPGA implementation (see Lab3A script)
open_solution "solution1" -flow_target vivado

# Set board to Pynq-Z2
set_part {xc7z020clg400-1}

create_clock -period 10 -name default

# (TODO: necessary?)
# config_interface -m_axi_latency 64

# Synthesis -- the IMPORTANT stuff!!
csynth_design

## C and Verilog co-simulation
## This usually takes a long time so it is commented
## You may uncomment it if necessary
# cosim_design

# export IP to Vivado
# Currently we're not exporting to FPGA so this command is commented;
# if you want to try it on FPGA, please uncomment it
# export_design -format ip_catalog

# Alternatively... -evaluate option enables C/RTL cosim
# export_design -evaluate verilog -format ip_catalog

exit
