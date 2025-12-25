open_project histogram_prj
set_top histogram_hls

add_files histogram.cpp
add_files params.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit

