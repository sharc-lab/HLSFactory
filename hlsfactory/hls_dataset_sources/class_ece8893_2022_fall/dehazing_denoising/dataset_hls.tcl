open_project dehazing_denoising_prj
set_top img_pipeline

add_files pipeline.cpp
add_files pipeline.h
add_files debayer.cpp
add_files whitebalance.cpp
add_files dehaze.cpp
add_files utils.cpp

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csynth_design
exit
