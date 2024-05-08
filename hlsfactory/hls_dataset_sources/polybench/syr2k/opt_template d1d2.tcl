#################TODO: check lp5 lp6###########################

set_directive_resource -core RAM_1P "syr2k" A
set_directive_resource -core RAM_1P "syr2k" B
set_directive_resource -core RAM_1P "syr2k" C
set_directive_interface -mode ap_fifo "syr2k" D_out


#dim = 4, for iob
array_partition,22,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" D_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" buff_A0
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" buff_A1
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" buff_B0
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" buff_B1
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" buff_C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" buff_D_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syr2k" tmp2
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" buff_A0
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" buff_A1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" buff_B0
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" buff_B1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" buff_C
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" buff_D_out
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syr2k" tmp2
set_directive_unroll -factor [factor] "syr2k/lprd_2"
set_directive_unroll -factor [factor] "syr2k/lpwr_2"

set_directive_pipeline "syr2k/lprd_2"
set_directive_pipeline "syr2k/lpwr_2"


#dim = 10, for loop1
loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] syr2k/[name]
set_directive_pipeline syr2k/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp7,pipeline,unroll,[1 2 4 8]
1,lp8,pipeline,unroll,[1 2 4 8]
2,lp8,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] syr2k/[name]
set_directive_pipeline syr2k/[name]