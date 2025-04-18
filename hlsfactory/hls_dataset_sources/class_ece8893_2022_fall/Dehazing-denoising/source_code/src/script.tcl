open_project project_1

# this is your top module 
#set_top matrix_mul
# set_top glob_info
set_top image_dehaze

# this is your file that to be synthesized as FPGA design
add_files colorbalance.cpp 
add_files ImageDehaze.cpp
# this is your testbench (specified by -tb option)
add_files -tb main.cpp

open_solution "solution_dehaze"

set_part {xczu3eg-sbva484-1-e}

create_clock -period 10 -name default

# synthesis
csynth_design

# -evaluate option enables C/RTL cosim
#export_design -evaluate verilog -format ip_catalog

# export IP to Vivado
# Currently we're not exporting to FPGA so this command is commented;
# if you want to try it on FPGA, please uncomment it
# export_design -format ip_catalog

exit
