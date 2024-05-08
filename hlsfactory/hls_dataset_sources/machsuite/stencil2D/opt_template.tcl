set_directive_resource -core RAM_1P_BRAM "stencil" orig
set_directive_resource -core RAM_1P_BRAM "stencil" sol
set_directive_resource -core RAM_1P_BRAM "stencil" filter

#dim = 5
array_partition,2,[1 2 4 8 16],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "stencil" orig
set_directive_array_partition -factor [factor] -type [type] -dim 1 "stencil" sol

#dim = 10 
loop_opt,3,2
0,stencil_label4,pipeline,unroll,[2 3]
1,stencil_label3,pipeline,unroll,[2 3]
2,stencil_label2,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] stencil/[name]
set_directive_pipeline stencil/[name]

