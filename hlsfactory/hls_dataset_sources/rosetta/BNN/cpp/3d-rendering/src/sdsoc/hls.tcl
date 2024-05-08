#=========================================================================
# hls.tcl
#=========================================================================
open_project hls.prj
set_top rendering

add_files rendering.cpp 

open_solution "solution1" -reset

set_part {xczu9eg-ffvb1156-2-i}
create_clock -period 10

config_rtl -reset state
source opt.tcl
# Apply optimizations
# source opt.tcl

csynth_design

exit
