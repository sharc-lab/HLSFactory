# Set project name and path
set project_name "build"
set src_path "." 

# Create a new HLS project
open_project "$src_path/$project_name"

# Add C++ source files
set source_files [list \
    "$src_path/FFT.cpp" \
    "$src_path/testbench.cpp" \
]
foreach file $source_files {
    if {[file exists $file]} {
        add_files $file
    } else {
        puts "Warning: File $file not found!"
    }
}

open_solution "FFT_${env(TARGET_FREQ)}MHz"
set_part $env(TARGET_PART_NUM)
set_top "FFT_TOP"
# Set the target frequency
create_clock -period [expr {1000.0 / $env(TARGET_FREQ)}]
config_compile -unsafe_math_optimizations 

add_files -tb "$src_path/testbench.cpp" 

# Run the simulation
csim_design
csynth_design
# report_directives
# cosim_design
export_design -flow syn -rtl verilog  
close_project

exit


