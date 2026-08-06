open_project ecg_baseline_prj
set_top tiled_conv

add_files conv1d.cpp
add_files conv.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
