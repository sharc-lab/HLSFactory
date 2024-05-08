set_directive_resource -core RAM_1P_BRAM "bbgemm" m1
set_directive_resource -core RAM_1P_BRAM "bbgemm" m2
set_directive_resource -core RAM_1P_BRAM "bbgemm" prod

#dim = 4
array_partition,3,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "bbgemm" m1
set_directive_array_partition -factor [factor] -type [type] -dim 1 "bbgemm" m2
set_directive_array_partition -factor [factor] -type [type] -dim 1 "bbgemm" prod

#dim = 17 
loop_opt,3,2
0,loopj,pipeline,unroll,[2 4 8 16 32 64]
1,loopk,pipeline,unroll,[2 4 8 16 32 64]
2,loopi,pipeline,unroll,[2 4 8 16]
set_directive_unroll -factor [factor] bbgemm/[name]
set_directive_pipeline bbgemm/[name]

