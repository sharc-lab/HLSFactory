open_project matrix_mul_prj
set_top matrix_mul

add_files matrix_mul.cpp
add_files params.h
add_files matrix_mul.h
add_files -tb matrix_mul_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
