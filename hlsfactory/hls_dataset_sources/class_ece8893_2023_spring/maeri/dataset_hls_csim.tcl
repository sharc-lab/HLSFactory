open_project maeri_prj
set_top maeri_v2_1

add_files maeri_v2_1.cpp
add_files maeri_v2_1.h

add_files -tb maeri_v2_1_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
