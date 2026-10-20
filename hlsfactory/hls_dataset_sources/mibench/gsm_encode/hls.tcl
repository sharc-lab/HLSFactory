open_project -reset hls_prj
add_files gsm_top.c
add_files add.c
add_files code.c
add_files gsm_encode.c
add_files long_term.c
add_files lpc.c
add_files preprocess.c
add_files rpe.c
add_files short_term.c
add_files table.c
add_files -tb tb.c
set_top gsm_encode_block
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
