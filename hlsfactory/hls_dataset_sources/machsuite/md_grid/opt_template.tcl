set_directive_resource -core RAM_1P_BRAM "md" n_points
set_directive_resource -core RAM_1P_BRAM "md" force
set_directive_resource -core RAM_1P_BRAM "md" position
set_directive_loop_tripcount -min 1 -max 4  md/loop_grid1_x
set_directive_loop_tripcount -min 1 -max 4  md/loop_grid1_y
set_directive_loop_tripcount -min 1 -max 4  md/loop_grid1_z
set_directive_loop_tripcount -min 1 -max 32  md/loop_p
set_directive_loop_tripcount -min 1 -max 32  md/loop_q

#dim = 3
array_partition,3,[1 2 4],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 0 "md" n_points
set_directive_array_partition -factor [factor] -type [type] -dim 0 "md" force
set_directive_array_partition -factor [factor] -type [type] -dim 0 "md" position

#dim = 11 
loop_opt,2,2
0,loop_q,pipeline,unroll,[2 4 8 16 32]
1,loop_p,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] md/[name]
set_directive_pipeline md/[name]
