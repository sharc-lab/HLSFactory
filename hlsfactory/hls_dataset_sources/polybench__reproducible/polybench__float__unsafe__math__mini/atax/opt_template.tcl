set_directive_resource -core RAM_1P "kernel_atax" A
set_directive_resource -core RAM_1P "kernel_atax" x

array_partition,5,[1 2 4 8],cyclic
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_atax" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_atax" tmp
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_atax" x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_atax" y
set_directive_unroll -factor [factor] "kernel_atax/lp0"

set_directive_pipeline "kernel_atax/lp0"

####
loop_opt,5,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
3,lp3,pipeline,unroll,[1 2 4 8]
4,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_atax/[name]
set_directive_pipeline kernel_atax/[name]
