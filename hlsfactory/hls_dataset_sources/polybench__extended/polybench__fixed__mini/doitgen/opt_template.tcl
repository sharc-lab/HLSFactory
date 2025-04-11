set_directive_resource -core RAM_1P "kernel_doitgen" A
set_directive_resource -core RAM_1P "kernel_doitgen" C4
set_directive_resource -core RAM_1P "kernel_doitgen" sum

array_partition,3,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 3 "kernel_doitgen" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_doitgen" C4
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_doitgen" sum

loop_opt,6,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp4,pipeline,unroll,[1 2 4 8]
3,lp4,,unroll,[1 2 4 8]
4,lp5,pipeline,unroll,[1 2 4 8]
5,lp5,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_doitgen/[name]
set_directive_pipeline kernel_doitgen/[name]