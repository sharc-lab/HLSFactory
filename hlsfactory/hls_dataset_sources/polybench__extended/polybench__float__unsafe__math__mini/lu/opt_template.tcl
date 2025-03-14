set_directive_resource -core RAM_1P "kernel_lu" A

array_partition,1,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_lu" A

loop_opt,6,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
3,lp4,pipeline,unroll,[1 2 4 8]
4,lp5,pipeline,unroll,[1 2 4 8]
5,lp5,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_lu/[name]
set_directive_pipeline kernel_lu/[name]

