# set HLS_PROJECT_DIR "fpga_gcn_qm9_vitis_hls_project"


# set script_dir [file dirname [info script]]
# set full_dir_path [file join $script_dir $HLS_PROJECT_DIR]
# if {[file exists $full_dir_path] && [file isdirectory $full_dir_path]} {
#     puts "The HLS project directory $HLS_PROJECT_DIR already exists. Exiting script."
#     return
# }

open_project -reset fpga_gcn_qm9_vitis_hls_project

add_files gnn_builder_lib.h
add_files model.h
add_files model.cpp


add_files -tb model.h
add_files -tb model_tb.cpp
add_files -tb tb_data

set_top fpga_gcn_qm9_top

open_solution "solution1" -flow_target vitis
set_part xczu9eg-ffvb1156-2-e
create_clock -period 3.33 -name default

csynth_design
cosim_design -setup
export_design -format ip_catalog -rtl verilog -display_name fpga_gcn_qm9