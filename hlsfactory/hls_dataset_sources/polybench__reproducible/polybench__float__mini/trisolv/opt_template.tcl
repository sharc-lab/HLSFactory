set_directive_resource -core RAM_1P "kernel_trisolv" L
set_directive_resource -core RAM_1P "kernel_trisolv" x
set_directive_resource -core RAM_1P "kernel_trisolv" b

array_partition,3,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_trisolv" L
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_trisolv" x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_trisolv" b

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_trisolv/[name]
set_directive_pipeline kernel_trisolv/[name]
