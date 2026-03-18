open_project spmv_prj
set_top spmv

add_files spmv.cpp
add_files params.h
add_files -tb spmv_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
