open_project optical_flow_prj
set_top optical_flow

add_files optical_flow.cpp -cflags "-I. -std=c++14"

add_files -tb host.cpp -cflags "-I. -std=c++14"

open_solution sol1
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default

csim_design
exit
