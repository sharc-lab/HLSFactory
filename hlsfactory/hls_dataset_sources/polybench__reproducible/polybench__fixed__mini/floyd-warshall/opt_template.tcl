set_directive_resource -core RAM_1P "kernel_floyd_warshall" path

array_partition,1,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_floyd_warshall" path

loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_floyd_warshall/[name]
set_directive_pipeline kernel_floyd_warshall/[name]
