open_project -reset hls_project
set_top vector_affine
add_files vector_affine.cpp
open_solution -reset solution1 -flow_target vivado
set_part {xc7z020clg400-1}
create_clock -period 10 -name default
csynth_design
exit
