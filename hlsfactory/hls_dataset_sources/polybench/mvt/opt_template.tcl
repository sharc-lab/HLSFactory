set_directive_resource -core RAM_1P "mvt" A
set_directive_resource -core RAM_1P "mvt" x1
set_directive_resource -core RAM_1P "mvt" x2
set_directive_resource -core RAM_1P "mvt" y1
set_directive_interface -mode ap_fifo "mvt" y2
set_directive_interface -mode ap_fifo "mvt" x1_out
set_directive_interface -mode ap_fifo "mvt" x2_out

#dim = 4, for iob
array_partition,8,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "mvt" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "mvt" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "mvt" buff_x1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "mvt" buff_x2
set_directive_array_partition -type [type] -factor [factor] -dim 1 "mvt" buff_y1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "mvt" buff_y2
set_directive_unroll -factor [factor] "mvt/lprd_2"
set_directive_unroll -factor [factor] "mvt/lpwr"

set_directive_pipeline "mvt/lprd_2"
set_directive_pipeline "mvt/lpwr"


#dim = 10, for loop1
loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] mvt/[name]
set_directive_pipeline mvt/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] mvt/[name]
set_directive_pipeline mvt/[name]
