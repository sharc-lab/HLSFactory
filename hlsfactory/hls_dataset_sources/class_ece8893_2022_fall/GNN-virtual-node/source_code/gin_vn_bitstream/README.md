# GIN_VN

This directory contains the modified code and other dependencies (binaries, makefile, tcl script, etc.) used for generating a bitstream to implement the EVA GIN-Virtual Node acceleration on a Pynq Z2 board (Note: make sure that you do not modify the directory structure you observe on GitHub to ensure seamless execution). The following steps can be followed to perform C-simulation and perform HLS synthesis:

## Performing C simulation:

- Open Terminal or a Powershell window
- Redirect to the directory where the code and the dependencies have been stored on your system
- Type _make_ and hit enter
- This will create an output file of the name _result_
- Execute this by typing _./result_
- The final prediction, along with the graph name is also stored in the file _HLS_optimized_output.txt_. This output will be used to verify the result obtained by running the code on the board

## Performing HLS synthesis:

- Open the _script.tcl_ file in any text editor.
- Uncomment the following lines by removing the _#_ and save changes:
<p align="center">
csynth_design
</p>

- Make sure the following lines are commented:

<p align="center">
cosim_design -trace_level all
</p>
<p align="center">
export_design -format ip_catalog
</p>
  
- Type _vitis_hls -f script.tcl_ on your command line and hit enter
- The reports generated through HLS synthesis can be accessed in the following directory
<p align="center">
project_1/solution/syn/report
</p>

## Running cosimulation:

- Open the _script.tcl_ file in any text editor.
- Uncomment the following lines by removing the # and save changes:
<p align="center">
cosim_design -trace_level all
</p>

- Make sure the following lines are commented:
<p align="center">
csynth_design
</p>
<p align="center">
export_design -format ip_catalog
</p>

- Type _vitis_hls -f script.tcl_ on your command line and hit enter
- The reports generated through co-simulation can be accessed in the following directory
<p align="center">
project_1/solution1/sim/report
</p>

## Exporting IP to Vivado:

- Open the _script.tcl_ file in any text editor.
- Uncomment the following lines by removing the # and save changes:
<p align="center">
export_design -format ip_catalog
</p>

- Make sure the following lines are commented:
<p align="center">
csynth_design
</p>
<p align="center">
cosim_design -trace_level all
</p>

- Type _last_year vitis_hls -f script.tcl_ on your command line and hit enter
- The exported IP generated can be accessed in the following directory
<p align="center">
project_1/solution1/impl
</p>

To generate a bitstream using Vivado, the steps in the following [tutorial](https://github.com/sharc-lab/FPGA_ECE8893/tree/main/tutorial#implement-the-adder-on-fpga-using-vivado) can be followed.
