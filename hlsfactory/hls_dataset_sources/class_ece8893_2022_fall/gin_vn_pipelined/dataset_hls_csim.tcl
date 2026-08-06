open_project gin_vn_pipelined_prj
set_top GIN_virtualnode_compute_one_graph

add_files GIN_virtualnode_compute.cpp
add_files dcl.hpp

add_files -tb main.cpp
add_files -tb load_weights_graph.cpp
add_files -tb gin-virtual_ep1_ed_embed_dim100.bin
add_files -tb gin-virtual_ep1_eps_dim100.bin
add_files -tb gin-virtual_ep1_mlp_1_bias_dim100.bin
add_files -tb gin-virtual_ep1_mlp_1_weights_dim100.bin
add_files -tb gin-virtual_ep1_mlp_2_bias_dim100.bin
add_files -tb gin-virtual_ep1_mlp_2_weights_dim100.bin
add_files -tb gin-virtual_ep1_nd_embed_dim100.bin
add_files -tb gin-virtual_ep1_pred_bias_dim100.bin
add_files -tb gin-virtual_ep1_pred_weights_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_embed_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_0_bias_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_0_weights_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_2_bias_dim100.bin
add_files -tb gin-virtual_ep1_virtualnode_mlp_2_weights_dim100.bin
add_files -tb g1_edge_attr.bin
add_files -tb g1_edge_list.bin
add_files -tb g1_info.txt
add_files -tb g1_node_feature.bin

open_solution sol1
set_part {xcu280-fsvh2892-2L-e}
create_clock -period 10 -name default

csim_design
exit
