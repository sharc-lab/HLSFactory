# Set project name and path
set project_name "build"
set src_path "."

set TARGET_FREQ 250
set TARGET_PART_NUM "xcvp1802-lsvc4072-3HP-e-S"

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

add_files -tb "$src_path/testbench.cpp"


open_solution "solution1"
set_part "$TARGET_PART_NUM"
set_top "FFT_TOP"
# Set the target frequency
create_clock -period [expr {1000.0 / $TARGET_FREQ}] -name clk
config_compile -unsafe_math_optimizations


if {[lindex $argv 0] == "csim"} {
    csim_design
} elseif {[lindex $argv 0] == "cosim"} {
    cosim_design
} elseif {[lindex $argv 0] == "syn"} {
    # config_op fmacc -impl auto -precision high
    csynth_design
} elseif {[lindex $argv 0] == "pnr"} {
    export_design -flow syn -rtl verilog
} elseif {[lindex $argv 0] == "toCosim"} {
    csim_design
    csynth_design
    cosim_design
} elseif {[lindex $argv 0] == "all"} {
    csim_design
    csynth_design
    cosim_design
    export_design -flow syn -rtl verilog
} else {
    puts "Error: [lindex $argv 0] is not a valid argument"
}

exit


