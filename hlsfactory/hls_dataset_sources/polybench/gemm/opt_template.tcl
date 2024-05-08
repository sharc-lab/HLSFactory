set_directive_resource -core RAM_1P "gemm" A
set_directive_resource -core RAM_1P "gemm" B
set_directive_resource -core RAM_1P "gemm" C
set_directive_interface -mode ap_fifo "gemm" D_out

#dim = 11, for iob
array_partition,16,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" D_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" buff_C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" buff_D_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "gemm" tmp1
set_directive_unroll -factor [factor] "gemm/lprd_2"
set_directive_unroll -factor [factor] "gemm/lpwr_2"


set_directive_pipeline "gemm/lprd_2"
set_directive_pipeline "gemm/lpwr_2"


#dim = 10, for loop1
loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] gemm/[name]
set_directive_pipeline gemm/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp4,pipeline,unroll,[1 2 4 8]
1,lp5,pipeline,unroll,[1 2 4 8]
2,lp5,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] gemm/[name]
set_directive_pipeline gemm/[name]
