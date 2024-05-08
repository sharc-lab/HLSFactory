#############TODO: NEED TO CHECK HOW TO WRITE LP5######################

set_directive_resource -core RAM_1P "gesummv" A
set_directive_resource -core RAM_1P "gesummv" B
set_directive_resource -core RAM_1P "gesummv" x
set_directive_interface -mode ap_fifo "gesummv" y_out

#dim = 4, for iob
array_partition,10,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gesummv" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gesummv" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gesummv" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gesummv" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 1 "gesummv" buff_x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "gesummv" buff_y_out
set_directive_array_partition -type [type] -factor [factor] -dim 1 "gesummv" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "gesummv" tmp2
set_directive_unroll -factor [factor] "gesummv/lprd_2"
set_directive_unroll -factor [factor] "gesummv/lpwr"


set_directive_pipeline "gesummv/lprd_2"
set_directive_pipeline "gesummv/lpwr"


#dim = 10, for loop1
loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] gesummv/[name]
set_directive_pipeline gesummv/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] gesummv/[name]
set_directive_pipeline gesummv/[name]
