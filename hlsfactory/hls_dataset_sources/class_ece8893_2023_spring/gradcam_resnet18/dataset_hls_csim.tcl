open_project gradcam_resnet18_prj
set_top resnet18

# Match GradCAM Makefile: sim.cpp #includes resnet18.cpp (pulls maxpool/cam).
# Compile the other translation units separately; do not also compile resnet18.cpp.
add_files conv1/conv1.cpp -cflags "-DCSIM_DEBUG"
add_files conv_ds/conv_ds.cpp -cflags "-DCSIM_DEBUG"
add_files conv_3x3_s1/conv_3x3_s1.cpp -cflags "-DCSIM_DEBUG"
add_files avg_pool/avg_pool.cpp -cflags "-DCSIM_DEBUG"
add_files linear_fc/linear_fc.cpp -cflags "-DCSIM_DEBUG"
add_files resize.cpp -cflags "-DCSIM_DEBUG"
add_files bundles.hpp
add_files util.h
add_files resize.hpp
add_files sim_util.hpp

add_files -tb sim.cpp -cflags "-DCSIM_DEBUG"
add_files -tb bin/conv1_bias.bin
add_files -tb bin/conv1_weight.bin
add_files -tb bin/fc_bias.bin
add_files -tb bin/fc_weight.bin
add_files -tb bin/l10_c1_bias.bin
add_files -tb bin/l10_c1_weight.bin
add_files -tb bin/l10_c2_bias.bin
add_files -tb bin/l10_c2_weight.bin
add_files -tb bin/l11_c1_bias.bin
add_files -tb bin/l11_c1_weight.bin
add_files -tb bin/l11_c2_bias.bin
add_files -tb bin/l11_c2_weight.bin
add_files -tb bin/l20_c1_bias.bin
add_files -tb bin/l20_c1_weight.bin
add_files -tb bin/l20_c2_bias.bin
add_files -tb bin/l20_c2_weight.bin
add_files -tb bin/l21_c1_bias.bin
add_files -tb bin/l21_c1_weight.bin
add_files -tb bin/l21_c2_bias.bin
add_files -tb bin/l21_c2_weight.bin
add_files -tb bin/l2_ds_bias.bin
add_files -tb bin/l2_ds_weight.bin
add_files -tb bin/l30_c1_bias.bin
add_files -tb bin/l30_c1_weight.bin
add_files -tb bin/l30_c2_bias.bin
add_files -tb bin/l30_c2_weight.bin
add_files -tb bin/l31_c1_bias.bin
add_files -tb bin/l31_c1_weight.bin
add_files -tb bin/l31_c2_bias.bin
add_files -tb bin/l31_c2_weight.bin
add_files -tb bin/l3_ds_bias.bin
add_files -tb bin/l3_ds_weight.bin
add_files -tb bin/l40_c1_bias.bin
add_files -tb bin/l40_c1_weight.bin
add_files -tb bin/l40_c2_bias.bin
add_files -tb bin/l40_c2_weight.bin
add_files -tb bin/l41_c1_bias.bin
add_files -tb bin/l41_c1_weight.bin
add_files -tb bin/l41_c2_bias.bin
add_files -tb bin/l41_c2_weight.bin
add_files -tb bin/l4_ds_bias.bin
add_files -tb bin/l4_ds_weight.bin
add_files -tb bin/weight_notebook.ipynb
add_files -tb expected_activations/n01739381_vine_snake/avgpool_out.bin
add_files -tb expected_activations/n01739381_vine_snake/cam_output.bin
add_files -tb expected_activations/n01739381_vine_snake/conv1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/input.bin
add_files -tb expected_activations/n01739381_vine_snake/l10_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l10_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l11_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l11_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l20_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l20_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l21_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l21_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l2_ds_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l30_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l30_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l31_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l31_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l3_ds_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l40_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l40_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l41_c1_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l41_c2_out.bin
add_files -tb expected_activations/n01739381_vine_snake/l4_ds_out.bin
add_files -tb expected_activations/n01739381_vine_snake/maxpool_out.bin
add_files -tb expected_activations/n01739381_vine_snake/output.bin

open_solution sol1
set_part {xc7z020clg400-1}
create_clock -period 10 -name default

csim_design
exit
