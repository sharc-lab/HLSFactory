open_project hls_[lindex $argv 0]
set_top forward
add_files src/[lindex $argv 0].cpp
add_files -tb src/[lindex $argv 0]_tb.cpp
add_files -tb data/
open_solution "solution1" -flow_target vivado
# set_part xcu200-fsgd2104-2-e
set_part {xczu9eg-ffvb1156-2-i}
create_clock -name ap_clk -period 3.33
config_compile -unsafe_math_optimizations
# config_interface -m_axi_addr64 -m_axi_offset off -register_io off
# if [lindex $argv 1] is csim, then csim_design, else if [lindex $argv 1] is csynth, then csynth_design
if {[lindex $argv 1] == "csim"} {
  csim_design
} elseif {[lindex $argv 1] == "cosim"} {
  cosim_design
} elseif {[lindex $argv 1] == "syn"} { 
  # config_op fmacc -impl auto -precision high
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