open_project analog_cim_baseline_prj
set_top tiled_cim_conv

add_files tiled_cim_conv.cpp
add_files cim_conv.cpp
add_files conv.h
add_files utils.cpp
add_files utils.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
