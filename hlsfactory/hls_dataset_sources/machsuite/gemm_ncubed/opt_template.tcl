set_directive_resource -core RAM_1P_BRAM "gemm" m1
set_directive_resource -core RAM_1P_BRAM "gemm" m2
set_directive_resource -core RAM_1P_BRAM "gemm" prod

#dim = 4
array_partition,3,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "gemm" m1
set_directive_array_partition -factor [factor] -type [type] -dim 1 "gemm" m2
set_directive_array_partition -factor [factor] -type [type] -dim 1 "gemm" prod

#dim = 12 
loop_opt,2,2
0,inner,pipeline,unroll,[2 4 8 16 32 64]
1,middle,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] gemm/[name]
set_directive_pipeline gemm/[name]

