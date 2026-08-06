open_project explainable_ml_prj
set_top tiled_conv

add_files tiled_conv.cpp
add_files gradient.h
add_files utils.h
add_files io.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
