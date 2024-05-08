set_directive_resource -core RAM_1P "bicg" A
set_directive_resource -core RAM_1P "bicg" r
set_directive_resource -core RAM_1P "bicg" p
set_directive_interface -mode ap_fifo "bicg" s_out
set_directive_interface -mode ap_fifo "bicg" q_out

#dim = 4, for iob
array_partition,9,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "bicg" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "bicg" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "bicg" buff_p
set_directive_array_partition -type [type] -factor [factor] -dim 1 "bicg" buff_r
set_directive_array_partition -type [type] -factor [factor] -dim 1 "bicg" buff_s_out
set_directive_array_partition -type [type] -factor [factor] -dim 1 "bicg" buff_q_out
set_directive_array_partition -type [type] -factor [factor] -dim 1 "bicg" A
set_directive_unroll -factor [factor] "bicg/lprd_2"
set_directive_unroll -factor [factor] "bicg/lpwr"

set_directive_pipeline "bicg/lprd_2"
set_directive_pipeline "bicg/lpwr"

#dim = 10, for loop1
loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] bicg/[name]
set_directive_pipeline bicg/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] bicg/[name]
set_directive_pipeline bicg/[name]