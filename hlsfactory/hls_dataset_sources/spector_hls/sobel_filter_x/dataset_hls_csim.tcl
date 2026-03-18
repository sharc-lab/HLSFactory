open_project sobel_filter_x_prj
set_top sobel_x

add_files sobel_subdimx.cpp
add_files params.h
add_files sobel.h
add_files -tb sobel_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
