open_project dsc_optimized_prj
set_top tiled_conv

add_files tiled_conv.cpp
add_files conv_3x3.cpp
add_files conv.h
add_files utils.cpp
add_files utils.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
