open_project eyeriss_prj
set_top convolution

add_files convolution.cpp
add_files convolution.h

add_files -tb testbench.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
