open_project mergesort_prj
set_top mergesort

add_files mergesort.cpp
add_files params.h
add_files -tb mergesort_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
