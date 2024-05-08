##########TODO: CHECK lp5 lp5############################

set_directive_resource -core RAM_1P "k2mm" A
set_directive_resource -core RAM_1P "k2mm" B
set_directive_resource -core RAM_1P "k2mm" C
set_directive_resource -core RAM_1P "k2mm" D
set_directive_interface -mode ap_fifo "k2mm" E_out

#dim = 22, for iob
array_partition,21,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" D
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" E_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" buff_C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" buff_D
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" buff_E_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k2mm" tmp2
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" buff_C
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" buff_D
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" buff_E_out
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "k2mm" tmp2
set_directive_unroll -factor [factor] "k2mm/lprd_2"
set_directive_unroll -factor [factor] "k2mm/lpwr_2"

set_directive_pipeline "k2mm/lprd_2"
set_directive_pipeline "k2mm/lpwr_2"


#dim = 10, for loop1
loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] k2mm/[name]
set_directive_pipeline k2mm/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp7,pipeline,unroll,[1 2 4 8]
1,lp8,pipeline,unroll,[1 2 4 8]
2,lp8,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] k2mm/[name]
set_directive_pipeline k2mm/[name]