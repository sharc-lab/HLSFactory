set_directive_resource -core RAM_1P "kernel_ludcmp" A
set_directive_resource -core RAM_1P "kernel_ludcmp" x
set_directive_resource -core RAM_1P "kernel_ludcmp" y

array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_ludcmp" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_ludcmp" b
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_ludcmp" x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_ludcmp" y

loop_opt,6,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
3,lp4,pipeline,unroll,[1 2 4 8]
4,lp5,pipeline,unroll,[1 2 4 8]
5,lp5,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_ludcmp/[name]
set_directive_pipeline kernel_ludcmp/[name]

loop_opt,3,2
0,lp6,pipeline,unroll,[1 2 4 8]
1,lp7,pipeline,unroll,[1 2 4 8]
2,lp7,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_ludcmp/[name]
set_directive_pipeline kernel_ludcmp/[name]

loop_opt,3,2
0,lp8,pipeline,unroll,[1 2 4 8]
1,lp9,pipeline,unroll,[1 2 4 8]
2,lp9,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_ludcmp/[name]
set_directive_pipeline kernel_ludcmp/[name]