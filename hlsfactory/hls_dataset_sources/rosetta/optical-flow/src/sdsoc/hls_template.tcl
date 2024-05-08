open_project -reset [prj_path]
set_top optical_flow
add_files /home/projects/ljohn/simplescalar/zhigang/rosetta/optical-flow/src/sdsoc/optical_flow.cpp
add_files /home/projects/ljohn/simplescalar/zhigang/rosetta/optical-flow/src/sdsoc/optical_flow.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_rtl -reset state
source [opt_tcl]
csynth_design
close_project

