open_project -reset hls_prj
add_files mix_cols.cpp
add_files -tb mix_cols_tb.cpp
set_top MixColumns
open_solution -reset solution
set_part {xc7vx690tffg1930-3}
create_clock -period 5

# phase: csim | syn (default syn)
if {[info exists ::argv] && [llength $::argv] >= 1} {
  set phase [lindex $::argv 0]
} else {
  set phase syn
}

if {$phase == "csim"} {
  csim_design
} elseif {$phase == "syn"} {
  csynth_design
} else {
  error "Unknown phase: $phase (expected csim or syn)"
}
exit
