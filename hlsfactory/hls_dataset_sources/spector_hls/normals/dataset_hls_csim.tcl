open_project normals_prj
set_top normals

add_files normals.cpp
add_files params.h
add_files -tb normals_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
