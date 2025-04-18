# GIN Virtual Node Pipelined HLS implementation
Note: The implementation supports inly 5 layers and embedding dimension 100. Modify declarations if the layers and dimension are changed
This directory contains the pipelined HLS implementation of GIN-Virtual node.
The pipelined HLS implementation has the following optimizations:
- MLP on graph nodes and virtual nodes fully pipelined
- Inter layer pipelining of MLP and message passing

## HLS implementation verification:

- build: make clean, make produces result executable
- Make sure that the graphs/ directory is parallel to gin_vn_pipelined_hls/ directory. Graph info and binaries are loaded from graphs/ which fetching the graphs
- Copy the following binaries to the current directory from ../gin_python/ as they are needed to load weights for the accelerator:
    - gin-virtual_ep1_ed_embed_dim100.bin
    - gin-virtual_ep1_eps_dim100.bin
    - gin-virtual_ep1_mlp_1_bias_dim100.bin
    - gin-virtual_ep1_mlp_1_weights_dim100.bin
    - gin-virtual_ep1_mlp_2_bias_dim100.bin
    - gin-virtual_ep1_mlp_2_weights_dim100.bin
    - gin-virtual_ep1_nd_embed_dim100.bin
    - gin-virtual_ep1_pred_bias_dim100.bin
    - gin-virtual_ep1_pred_weights_dim100.bin
    - gin-virtual_ep1_virtualnode_embed_dim100.bin
    - gin-virtual_ep1_virtualnode_mlp_0_bias_dim100.bin
    - gin-virtual_ep1_virtualnode_mlp_0_weights_dim100.bin
    - gin-virtual_ep1_virtualnode_mlp_2_bias_dim100.bin
    - gin-virtual_ep1_virtualnode_mlp_2_weights_dim100.bin
- Running ./result generates HLS_optimized_output.txt that should be verified against Golden_C_output.txt generated in gin_goldenC/ directory.

## HLS Sythesis:
- Open the _script.tcl_ file in any text editor.
- Make sure the following lines are uncommented:
<p align="center">
csynth_design
</p>

- Make sure the following lines are commented:

<p align="center">
export_design -evaluate verilog -format ip_catalog
</p>
<p align="center">
export_design -format ip_catalog
</p>

- Run the command: vitis_hls -f script.tcl to synthesize HLS implementation and obtain latency and resource utlization estimates
