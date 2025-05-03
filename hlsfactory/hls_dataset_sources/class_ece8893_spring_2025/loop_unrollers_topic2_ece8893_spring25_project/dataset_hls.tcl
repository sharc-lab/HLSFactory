open_project project_1

open_solution "solution1"

# set top function of the HLS design
set_top compute_tracks_HLS

# add source file
add_files top.cpp

# add testbench
add_files -tb host.cpp

# add data file
# grab every .bin in any event directory two levels under ./converted
set all_event_bins [glob -nocomplain ./converted/*/event*/*.bin]

# add them as test-bench data files
add_files -tb $all_event_bins

# FPGA part and clock configuration
set_part {xczu3eg-sbva484-1-e}

# default frequency is 100 MHz
#create_clock -period 4 -name default

# C synthesis for HLS design, generating RTL
csynth_design

exit

