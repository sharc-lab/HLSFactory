set_directive_resource -core RAM_1P "kernel_2mm" A
set_directive_resource -core RAM_1P "kernel_2mm" B
set_directive_resource -core RAM_1P "kernel_2mm" C
set_directive_resource -core RAM_1P "kernel_2mm" D

array_partition,5,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_2mm" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_2mm" B
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_2mm" C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_2mm" D
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_2mm" tmp

loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_2mm/[name]
set_directive_pipeline kernel_2mm/[name]

loop_opt,3,2
0,lp5,pipeline,unroll,[1 2 4 8]
1,lp6,pipeline,unroll,[1 2 4 8]
2,lp6,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_2mm/[name]
set_directive_pipeline kernel_2mm/[name]




