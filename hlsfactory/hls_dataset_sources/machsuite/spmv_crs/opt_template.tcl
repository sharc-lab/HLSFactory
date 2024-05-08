set_directive_resource -core RAM_1P_BRAM "spmv" val
set_directive_resource -core RAM_1P_BRAM "spmv" rowDelimiters
set_directive_resource -core RAM_1P_BRAM "spmv" vec
set_directive_resource -core RAM_1P_BRAM "spmv" out

#dim = 4
array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "spmv" val
set_directive_array_partition -factor [factor] -type [type] -dim 1 "spmv" rowDelimiters
set_directive_array_partition -factor [factor] -type [type] -dim 1 "spmv" vec
set_directive_array_partition -factor [factor] -type [type] -dim 1 "spmv" out

#dim = 12 
loop_opt,2,2
0,spmv_2,pipeline,unroll,[2 4 8 16 32 64]
1,spmv_1,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] spmv/[name]
set_directive_pipeline spmv/[name]
