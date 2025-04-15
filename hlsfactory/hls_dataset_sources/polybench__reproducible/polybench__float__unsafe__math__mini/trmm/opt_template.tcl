set_directive_resource -core RAM_1P "kernel_trmm" A
set_directive_resource -core RAM_1P "kernel_trmm" B

array_partition,2,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_trmm" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_trmm" B

loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_trmm/[name]
set_directive_pipeline kernel_trmm/[name]
