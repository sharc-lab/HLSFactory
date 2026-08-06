open_project flat_systolic_array_prj
set_top systolic_array

add_files systolic.cpp
add_files systolic.h

add_files -tb systolic_test.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
