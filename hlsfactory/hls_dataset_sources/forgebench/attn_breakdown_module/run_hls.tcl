# Auto-generated TCL file for HLS
open_project -reset project_1

set_top top

add_files  top.cpp
add_files -tb top.h

open_solution "solution1"

set_part xczu9eg-ffvb1156-2-e

create_clock -period 10 -name default

csynth_design

export_design -format ip_catalog -flow impl

exit