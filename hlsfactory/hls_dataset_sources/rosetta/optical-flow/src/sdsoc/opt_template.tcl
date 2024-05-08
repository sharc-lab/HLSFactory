#dim = 4
loop_opt,1,7
0,FRAME,,unroll,[2 4 8]
set_directive_array_partition -factor [factor] -type cyclic optical_flow frame1_a
set_directive_array_partition -factor [factor] -type cyclic optical_flow frame2_a
set_directive_array_partition -factor [factor] -type cyclic optical_flow frame4_a
set_directive_array_partition -factor [factor] -type cyclic optical_flow frame5_a
set_directive_array_partition -factor [factor] -type cyclic optical_flow frame3_a
set_directive_array_partition -factor [factor] -type cyclic optical_flow frame3_b
set_directive_unroll -factor [factor] optical_flow/FRAMES_CP_INNER

#dim = 4 
loop_opt,1,5
0,GRAD_CAL,,unroll,[2 4 8]
set_directive_unroll -factor [factor] gradient_xy_calc/GRAD_XY_INNER
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" gradient_x
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" gradient_y
set_directive_unroll -factor [factor] gradient_z_calc/GRAD_Z_INNER
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" gradient_z

#dim = 4 
loop_opt,1,6
0,GRAD_WEIGHT,,unroll,[2 4 8]
set_directive_unroll -factor [factor] gradient_weight_y/GRAD_WEIGHT_Y_INNER
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" y_filtered
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" filtered_gradient
set_directive_unroll -factor [factor] gradient_weight_x/GRAD_WEIGHT_X_INNER
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" out_product
set_directive_unroll -factor [factor] outer_product/OUTER_INNER


#dim = 4 
loop_opt,1,5
0,TENSOR,,unroll,[2 4 8]
set_directive_unroll -factor [factor] tensor_weight_y/TENSOR_WEIGHT_Y_INNER
set_directive_unroll -factor [factor] tensor_weight_x/TENSOR_WEIGHT_X_INNER
set_directive_unroll -factor [factor] flow_calc/FLOW_INNER
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" tensor_y
set_directive_array_partition -factor [factor] -type block -dim 0 "optical_flow" tensor
