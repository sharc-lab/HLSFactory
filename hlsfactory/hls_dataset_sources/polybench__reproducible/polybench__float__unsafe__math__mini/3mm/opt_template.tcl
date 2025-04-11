set_directive_resource -core RAM_1P "kernel_3mm" A
set_directive_resource -core RAM_1P "kernel_3mm" B
set_directive_resource -core RAM_1P "kernel_3mm" C
set_directive_resource -core RAM_1P "kernel_3mm" D
set_directive_resource -core RAM_1P "kernel_3mm" E
set_directive_resource -core RAM_1P "kernel_3mm" F
set_directive_resource -core RAM_1P "kernel_3mm" G

array_partition,7,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_3mm" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_3mm" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_3mm" C
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_3mm" D
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_3mm" E
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_3mm" F
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_3mm" G

loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_3mm/[name]
set_directive_pipeline kernel_3mm/[name]

loop_opt,3,2
0,lp5,pipeline,unroll,[1 2 4 8]
1,lp6,pipeline,unroll,[1 2 4 8]
2,lp6,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_3mm/[name]
set_directive_pipeline kernel_3mm/[name]

loop_opt,3,2
0,lp8,pipeline,unroll,[1 2 4 8]
1,lp9,pipeline,unroll,[1 2 4 8]
2,lp9,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_3mm/[name]
set_directive_pipeline kernel_3mm/[name]



