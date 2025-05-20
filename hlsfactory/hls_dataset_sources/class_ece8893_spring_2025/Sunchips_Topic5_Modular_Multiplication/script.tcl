open_project project_1

# set top function of the HLS design
set_top mod_mul

# add source file
add_files top.cpp

# add testbench
add_files -tb host.cpp

open_solution "solution1"

# FPGA part and clock configuration
set_part {xcvu19p-fsva3824-2-e}

# default frequency is 100 MHz
#create_clock -period 4 -name default

# C synthesis for HLS design, generating RTL
csynth_design

# C/RTL co-simulation; can be commented if not needed
cosim_design

# export generated RTL as an IP; can be commented if not needed
# export_design -format ip_catalog -flow syn 
export_design -format ip_catalog -flow impl 



exit
