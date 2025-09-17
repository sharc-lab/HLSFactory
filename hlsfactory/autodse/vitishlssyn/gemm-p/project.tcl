

if { ![info exists ::env(HLS_LLVM_PLUGIN_DIR)] } {
  # Use plugin example directory as default build directory
  set ::env(HLS_LLVM_PLUGIN_DIR) [file normalize ../llvmpass]
}

if { ![file exists $::env(HLS_LLVM_PLUGIN_DIR)/LLVMMemoryPartition.so] } {
  error "Must build LLVMMemoryPartition.so before running this example"
}
set ::LLVM_CUSTOM_CMD {$LLVM_CUSTOM_OPT -always-inline -mem2reg -gvn -instcombine -mem2reg -gvn -indvars  -load $::env(HLS_LLVM_PLUGIN_DIR)/LLVMMemoryPartition.so -enable-falcon-test=true -falcon-memory-partition $LLVM_CUSTOM_INPUT -o $LLVM_CUSTOM_OUTPUT}

# Set project name and path
set project_name "project"
set src_path "." 

# Create a new HLS project
open_project -reset "$src_path/$project_name"

# Add C++ source files
add_files "testbench.cpp"
add_files "gemm-p.cpp"
add_files "gemm-p.h"
add_files -tb "testbench.cpp"

open_solution "solution"
set_part $env(TARGET_PART_NUM)
set_top "kernel_gemm"
# Set the target frequency
create_clock -period [expr {1000.0 / $env(TARGET_FREQ)}]
config_compile -unsafe_math_optimizations 

# Run the simulation
csynth_design
# export_design -flow syn -rtl verilog  
close_project

exit



