# Create a project
open_project flat

# Set the top-level function
set_top FlatDataflow

# Add design files
add_files flat.cpp
add_files utils.cpp
add_files systolic_array.cpp
# add_files flat.cpp 
# Add test bench & files
# add_files -tb sim.cpp
# # Add memory files
# add_files -tb ./bin/query.bin
# add_files -tb ./bin/key.bin
# add_files -tb ./bin/bias.bin
# add_files -tb ./bin/value.bin
# add_files -tb ./bin/golden_output_one.bin


# ########################################################
# Create a solution
open_solution "solution1" -flow_target vivado
# Define technology and clock rate
set_part {xc7z020-clg400-1}
create_clock -period 10 -name default

csynth_design

# Source x_hls.tcl to determine which steps to execute
# source x_hls.tcl
# csim_design
# Set any optimization directives
# End of directives
# if {$hls_exec == 1} {
# 	# Run Synthesis and Exit
# 	csynth_design
	
# } elseif {$hls_exec == 2} {
# 	# Run Synthesis, RTL Simulation and Exit
# 	csynth_design
	
# 	cosim_design
# } elseif {$hls_exec == 3} { 
# 	# Run Synthesis, RTL Simulation, RTL implementation and Exit
# 	csynth_design
	
# 	cosim_design
# 	export_design
# } else {
# 	# Default is to exit after setup
# 	csynth_design
# }

exit
