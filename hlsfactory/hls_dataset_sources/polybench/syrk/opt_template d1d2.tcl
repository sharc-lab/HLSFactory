set_directive_resource -core RAM_1P "syrk" A
set_directive_resource -core RAM_1P "syrk" B
set_directive_interface -mode ap_fifo "syrk" C_out


#dim = 4, for iob
array_partition,13,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" C_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" buff_A0
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" buff_A1
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "syrk" buff_C_out
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syrk" buff_A0
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syrk" buff_A1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syrk" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 1 "syrk" buff_C_out
set_directive_unroll -factor [factor] "syrk/lprd_2"
set_directive_unroll -factor [factor] "syrk/lpwr_2"

set_directive_pipeline "syrk/lprd_2"
set_directive_pipeline "syrk/lpwr_2"


#dim = 10, for loop1
loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] syrk/[name]
set_directive_pipeline syrk/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp4,pipeline,unroll,[1 2 4 8]
1,lp5,pipeline,unroll,[1 2 4 8]
2,lp5,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] syrk/[name]
set_directive_pipeline syrk/[name]