set_directive_resource -core RAM_1P_BRAM "ellpack" nzval
set_directive_resource -core RAM_1P_BRAM "ellpack" cols
set_directive_resource -core RAM_1P_BRAM "ellpack" vec
set_directive_resource -core RAM_1P_BRAM "ellpack" out

#dim = 4
array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ellpack" nzval
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ellpack" cols
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ellpack" vec
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ellpack" out

#dim = 13 
loop_opt,2,2
0,ellpack_2,pipeline,unroll,[2 4 8 10]
1,ellpack_1,pipeline,unroll,[2 4 8 16 32 64 128 256]
set_directive_unroll -factor [factor] ellpack/[name]
set_directive_pipeline ellpack/[name]
