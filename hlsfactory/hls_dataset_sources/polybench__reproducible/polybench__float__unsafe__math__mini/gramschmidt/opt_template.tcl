set_directive_resource -core RAM_1P "kernel_gramschmidt" A
set_directive_resource -core RAM_1P "kernel_gramschmidt" R
set_directive_resource -core RAM_1P "kernel_gramschmidt" Q

array_partition,3,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gramschmidt" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_gramschmidt" R
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gramschmidt" Q

loop_opt,10,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
3,lp3,pipeline,unroll,[1 2 4 8]
4,lp3,,unroll,[1 2 4 8]
5,lp4,pipeline,unroll,[1 2 4 8]
6,lp5,pipeline,unroll,[1 2 4 8]
7,lp5,,unroll,[1 2 4 8]
8,lp6,pipeline,unroll,[1 2 4 8]
9,lp6,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_gramschmidt/[name]
set_directive_pipeline kernel_gramschmidt/[name]