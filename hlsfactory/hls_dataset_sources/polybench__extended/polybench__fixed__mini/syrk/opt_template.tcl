set_directive_resource -core RAM_1P "kernel_syrk" A
set_directive_resource -core RAM_1P "kernel_syrk" C

array_partition,2,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" C

loop_opt,6,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
3,lp3,pipeline,unroll,[1 2 4 8]
4,lp4,pipeline,unroll,[1 2 4 8]
5,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_syrk/[name]
set_directive_pipeline kernel_syrk/[name]