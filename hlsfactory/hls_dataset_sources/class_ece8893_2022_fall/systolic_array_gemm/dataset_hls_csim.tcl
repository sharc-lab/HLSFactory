open_project systolic_array_gemm_prj
set_top matrix_mul

add_files Source.cpp
add_files dcl.h

add_files -tb TestBench.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
