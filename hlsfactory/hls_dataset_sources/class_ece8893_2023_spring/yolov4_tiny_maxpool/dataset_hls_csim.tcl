open_project yolov4_tiny_maxpool_prj
set_top tiled_maxpool2D

add_files conv.h
add_files utils.cpp
add_files utils.h
add_files maxpool2D.cpp
add_files tiled_maxpool2D.cpp

add_files -tb sim.cpp
add_files -tb bin/maxpool_input.bin
add_files -tb bin/maxpool_output.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
