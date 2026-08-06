open_project dehazing_denoising_prj
set_top img_pipeline

add_files pipeline.cpp
add_files pipeline.h
add_files debayer.cpp
add_files whitebalance.cpp
add_files dehaze.cpp
add_files utils.cpp

add_files -tb main.cpp
add_files -tb input_bayer.raw
add_files -tb input2_bayer.raw

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
