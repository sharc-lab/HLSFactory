##########TODO: CHECK lp5 lp5############################

set_directive_resource -core RAM_1P "k3mm" A
set_directive_resource -core RAM_1P "k3mm" B
set_directive_resource -core RAM_1P "k3mm" C
set_directive_resource -core RAM_1P "k3mm" D
set_directive_interface -mode ap_fifo "k3mm" E_out

#dim = 4, for iob
array_partition,14,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" D
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" E_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" buff_B
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" buff_C
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" buff_D
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" buff_E_out
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 2 "k3mm" tmp2
set_directive_unroll -factor [factor] "k3mm/lprd_2"
set_directive_unroll -factor [factor] "k3mm/lpwr_2"

set_directive_pipeline "k3mm/lprd_2"
set_directive_pipeline "k3mm/lpwr_2"


#dim = 10, for loop1
loop_opt,3,2
0,lp2,pipeline,unroll,[1 2 4 8]
1,lp3,pipeline,unroll,[1 2 4 8]
2,lp3,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] k3mm/[name]
set_directive_pipeline k3mm/[name]


#should be dim = 10, for loop2
loop_opt,3,2
0,lp7,pipeline,unroll,[1 2 4 8]
1,lp8,pipeline,unroll,[1 2 4 8]
2,lp8,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] k3mm/[name]
set_directive_pipeline k3mm/[name]
