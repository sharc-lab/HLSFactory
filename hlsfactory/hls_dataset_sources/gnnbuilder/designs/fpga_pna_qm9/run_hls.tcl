open_project -reset fpga_pna_qm9_vitis_hls_project

add_files gnn_builder_lib.h
add_files model.h
add_files model.cpp

add_files -tb model_tb.cpp
add_files -tb tb_data

set_top fpga_pna_qm9_top

open_solution "solution1" -flow_target vitis
set_part xcu280-fsvh2892-2L-e
create_clock -period 3.33 -name default

csynth_design
cosim_design -setup
export_design -format ip_catalog -rtl verilog -display_name fpga_pna_qm9