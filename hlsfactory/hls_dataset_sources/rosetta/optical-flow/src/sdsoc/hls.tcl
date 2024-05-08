open_project -reset original
set_top optical_flow
add_files /home/projects/ljohn/simplescalar/zhigang/rosetta/optical-flow/src/sdsoc/optical_flow_original.cpp
add_files /home/projects/ljohn/simplescalar/zhigang/rosetta/optical-flow/src/sdsoc/optical_flow.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
config_rtl -reset state
csynth_design
close_project
exit
