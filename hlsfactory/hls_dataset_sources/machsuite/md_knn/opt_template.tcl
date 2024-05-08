set_directive_resource -core RAM_1P_BRAM "md_kernel" force_x
set_directive_resource -core RAM_1P_BRAM "md_kernel" force_y
set_directive_resource -core RAM_1P_BRAM "md_kernel" force_z
set_directive_resource -core RAM_1P_BRAM "md_kernel" position_x
set_directive_resource -core RAM_1P_BRAM "md_kernel" position_y
set_directive_resource -core RAM_1P_BRAM "md_kernel" position_z
set_directive_resource -core RAM_1P_BRAM "md_kernel" NL


#dim = 4
array_partition,7,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" force_x
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" force_y
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" force_z
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" position_x
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" position_y
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" position_z
set_directive_array_partition -factor [factor] -type [type] -dim 1 "md_kernel" NL

#dim = 11 
loop_opt,2,2
0,loop_j,pipeline,unroll,[2 4 8 16]
1,loop_i,pipeline,unroll,[2 4 8 16 32 64]
set_directive_unroll -factor [factor] md_kernel/[name]
set_directive_pipeline md_kernel/[name]
