open_project hls_[lindex $argv 0]

add_files kernel.cpp
add_files -tb kernel_tb.cpp
# add_files -tb data/

open_solution "solution1" -flow_target vivado

set_part xczu9eg-ffvb1156-2-e
create_clock -period 10 -name default
config_compile -unsafe_math_optimizations

set_top whetstone_kernel


if {[lindex $argv 1] == "csim"} {
  csim_design
} elseif {[lindex $argv 1] == "cosim"} {
  cosim_design
} elseif {[lindex $argv 1] == "syn"} {
  csynth_design
} elseif {[lindex $argv 1] == "pnr"} {
  export_design -flow impl
  get_clock_period -name ap_clk -ns
} elseif {[lindex $argv 1] == "toCosim"} {
  csim_design
  csynth_design
  cosim_design
} elseif {[lindex $argv 1] == "all"} {
  csim_design
  csynth_design
  cosim_design
  export_design -flow impl
  get_clock_period -name ap_clk -ns
} else {
  puts "Error: [lindex $argv 1] is not a valid argument"
}
exit