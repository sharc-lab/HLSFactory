set_directive_resource -core RAM_1P "kernel_bicg" A
set_directive_resource -core RAM_1P "kernel_bicg" r
set_directive_resource -core RAM_1P "kernel_bicg" p

array_partition,6,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_bicg" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_bicg" r
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_bicg" p
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_bicg" s
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_bicg" q
set_directive_unroll -factor [factor] "kernel_bicg/lp0"

set_directive_pipeline "kernel_bicg/lp0"

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_bicg/[name]
set_directive_pipeline kernel_bicg/[name]
