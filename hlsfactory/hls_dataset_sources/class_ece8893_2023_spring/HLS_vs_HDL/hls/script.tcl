# TCL commands for batch-mode HLS
open_project proj
set_top gausFilter

add_files Sobel.h
add_files Sobel.cpp
add_files utils.h
add_files utils.cpp
add_files gausFilter.h
add_files gausFilter.cpp

open_solution "solution1" -flow_target vivado
set_part {xc7z020clg400-1}
create_clock -period 10 -name default



## C code synthesis to generate Verilog code
csynth_design


#export_design -format ip_catalog

exit
