open_project -reset hls_prj
set_top aes256_encrypt_ecb
add_files aes_table.c
add_files aes.h
open_solution -reset solution
set_part xczu9eg-ffvb1156-2-i
create_clock -period 10 -name default
source opt.tcl
csynth_design
close_project

