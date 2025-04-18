open_project project_1

set_top GIN_virtualnode_compute_one_graph

add_files GIN_virtualnode_compute.cpp
add_files dcl.hpp

add_files -tb g1_edge_attr.bin
add_files -tb g1_edge_list.bin
add_files -tb g1_info.txt
add_files -tb g1_node_feature.bin
add_files -tb gin-virtual_ep1_ed_embed_dim100.bin
add_files -tb gin-virtual_ep1_eps_dim100.bin
add_files -tb gin-virtual_ep1_mlp_1_bias_dim100.bin
add_files -tb gin-virtual_ep1_mlp_1_weights_dim100.bin
add_files -tb gin-virtual_ep1_mlp_2_bias_dim100.bin
add_files -tb gin-virtual_ep1_mlp_2_weights_dim100.bin
add_files -tb gin-virtual_ep1_nd_embed_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_embed_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_0_bias_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_0_weights_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_2_bias_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_2_weights_dim100.bin
add_files -tb gin-virtual_ep1_pred_weights_dim100.bin
add_files -tb gin-virtual_ep1_pred_bias_dim100.bin
add_files -tb load_weights_graph.cpp
add_files -tb main.cpp

open_solution "solution1"
#set_part {xczu3eg-sbva484-1-e}
#set_part {xcu280-fsvh2892-2L-e}
set_part {xc7z020clg400-1}
create_clock -period 10ns -name default
#csynth_design
#cosim_design -trace_level all
# export IP to Vivado
# Currently we're not exporting to FPGA so this command is commented;
# if you want to try it on FPGA, please uncomment it
#export_design -format ip_catalog
exit
