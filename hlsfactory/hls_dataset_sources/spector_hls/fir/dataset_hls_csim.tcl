open_project fir_prj
set_top fir_hls

add_files fir_hls.cpp
add_files params.h
add_files fir_hls.h
add_files -tb fir_tb.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
