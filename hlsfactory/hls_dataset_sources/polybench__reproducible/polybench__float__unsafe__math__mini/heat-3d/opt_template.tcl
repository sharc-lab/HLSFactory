set_directive_resource -core RAM_1P "kernel_heat_3d" A
set_directive_resource -core RAM_1P "kernel_heat_3d" B

array_partition,2,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 3 "kernel_heat_3d" A
set_directive_array_partition -type [type] -factor [factor] -dim 3 "kernel_heat_3d" B

loop_opt,6,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
3,lp6,pipeline,unroll,[1 2 4 8]
4,lp7,pipeline,unroll,[1 2 4 8]
5,lp7,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_heat_3d/[name]
set_directive_pipeline kernel_heat_3d/[name]





