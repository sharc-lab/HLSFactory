set_directive_resource -core RAM_1P "kernel_mvt" A
set_directive_resource -core RAM_1P "kernel_mvt" x1
set_directive_resource -core RAM_1P "kernel_mvt" x2
set_directive_resource -core RAM_1P "kernel_mvt" y_1
set_directive_interface -mode ap_fifo "kernel_mvt" y_2

array_partition,5,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_mvt" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_mvt" x1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_mvt" x2
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_mvt" y_1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_mvt" y_2

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_mvt/[name]
set_directive_pipeline kernel_mvt/[name]

loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_mvt/[name]
set_directive_pipeline kernel_mvt/[name]
