open_project top_prj
set_top top

add_files Accel.cpp -cflags "-I. -std=c++14 -DHLS_COMPILE"

open_solution sol1
set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10 -name default
config_rtl -reset state

csynth_design
exit
