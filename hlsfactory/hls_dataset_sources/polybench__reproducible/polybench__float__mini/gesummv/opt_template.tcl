set_directive_resource -core RAM_1P "kernel_gesummv" A
set_directive_resource -core RAM_1P "kernel_gesummv" B
set_directive_resource -core RAM_1P "kernel_gesummv" x

array_partition,5,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_gesummv" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_gesummv" B
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gesummv" tmp
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gesummv" x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gesummv" y

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_gesummv/[name]
set_directive_pipeline kernel_gesummv/[name]


