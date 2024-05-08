set_directive_resource -core RAM_1P_BRAM "stencil3d" orig
set_directive_resource -core RAM_1P_BRAM "stencil3d" sol

#dim = 6
array_partition,4,[1 2 4 8 16 32],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "stencil3d" orig
set_directive_array_partition -factor [factor] -type [type] -dim 1 "stencil3d" sol

#dim = 6 
loop_opt,1,6
0,height_col_row,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] stencil3d/height_bound_col
set_directive_unroll -factor [factor] stencil3d/col_bound_height
set_directive_unroll -factor [factor] stencil3d/row_bound_height
set_directive_pipeline stencil3d/height_bound_col
set_directive_pipeline stencil3d/col_bound_height
set_directive_pipeline stencil3d/row_bound_height

#dim = 9 
loop_opt,2,2
0,loop_row,pipeline,unroll,[2 4 8 14]
1,loop_col,pipeline,unroll,[2 4 8 16]
set_directive_unroll -factor [factor] stencil3d/[name]
set_directive_pipeline stencil3d/[name]
