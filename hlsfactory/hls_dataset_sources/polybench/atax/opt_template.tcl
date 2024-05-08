set_directive_resource -core RAM_1P "atax" A
set_directive_resource -core RAM_1P "atax" x
set_directive_interface -mode ap_fifo "atax" y_out

#dim = 4, for iob
array_partition,7,[1 2 4 8],cyclic
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" buff_x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" buff_y_out
set_directive_unroll -factor [factor] "atax/lprd_2"
set_directive_unroll -factor [factor] "atax/lpwr_1"

set_directive_pipeline "atax/lprd_2"
set_directive_pipeline "atax/lpwr_1"

#dim = 10, for loop1
loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] atax/[name]
set_directive_pipeline atax/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] atax/[name]
set_directive_pipeline atax/[name]
