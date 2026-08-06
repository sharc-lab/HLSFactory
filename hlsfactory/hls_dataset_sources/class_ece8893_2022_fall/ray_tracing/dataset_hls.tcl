open_project ray_tracing_prj
set_top render

add_files common.cpp
add_files common.h
add_files config.h
add_files fxp_sqrt.h
add_files geometry.h
add_files trianglemesh.cpp
add_files trianglemesh.h

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 10 -name default

csynth_design
exit
