# Auto-generated TCL file for HLS
open_project -reset project_1

set_top top

add_files  top.cpp
add_files -tb tb_top.cpp
add_files -tb top.h

add_files -tb DRAM_1.txt
add_files -tb DRAM_2.txt
add_files -tb DRAM_3.txt
add_files -tb DRAM_4.txt
add_files -tb DRAM_5.txt
add_files -tb DRAM_10.txt
add_files -tb DRAM_11.txt
add_files -tb DRAM_12.txt
open_solution "solution1"

set_part xczu9eg-ffvb1156-2-e

create_clock -period 10 -name default

csynth_design

exit