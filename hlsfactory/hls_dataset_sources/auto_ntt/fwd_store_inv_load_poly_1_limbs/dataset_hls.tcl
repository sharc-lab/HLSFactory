open_project fwd_store_inv_load_poly_1_limbs_prj
set_top fwd_store_inv_load_poly_1_limbs

add_files fwd_store_inv_load_poly_1_limbs.cpp -cflags "-Isrc/ -std=c++11"

open_solution sol1
set_part {xczu3eg-sbva484-1-e}
create_clock -period 4 -name default

csynth_design
exit
