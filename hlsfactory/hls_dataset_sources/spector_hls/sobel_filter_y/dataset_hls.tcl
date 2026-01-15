open_project sobel_filter_y_prj
set_top sobel_y

add_files sobel_subdimy.cpp
add_files params.h
add_files sobel.h

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
