set_directive_resource -core RAM_1P_BRAM "needwun" SEQA
set_directive_resource -core RAM_1P_BRAM "needwun" SEQB
set_directive_resource -core RAM_1P_BRAM "needwun" alignedA
set_directive_resource -core RAM_1P_BRAM "needwun" alignedB
set_directive_resource -core RAM_1P_BRAM "needwun" M
set_directive_resource -core RAM_1P_BRAM "needwun" ptr

#dim = 4
array_partition,6,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "needwun" SEQA
set_directive_array_partition -factor [factor] -type [type] -dim 1 "needwun" SEQB
set_directive_array_partition -factor [factor] -type [type] -dim 1 "needwun" alignedA
set_directive_array_partition -factor [factor] -type [type] -dim 1 "needwun" alignedB
set_directive_array_partition -factor [factor] -type [type] -dim 1 "needwun" M
set_directive_array_partition -factor [factor] -type [type] -dim 1 "needwun" ptr

#dim = 6
loop_opt,1,8
0,init,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] needwun/init_row
set_directive_unroll -factor [factor] needwun/init_col
set_directive_unroll -factor [factor] needwun/pad_a
set_directive_unroll -factor [factor] needwun/pad_b
set_directive_pipeline needwun/init_row
set_directive_pipeline needwun/init_col
set_directive_pipeline needwun/pad_a
set_directive_pipeline needwun/pad_b

#dim = 7
loop_opt,1,2
0,fill_in,pipeline,unroll,[2 4 8 16 32 64]
set_directive_unroll -factor [factor] needwun/fill_in
set_directive_pipeline needwun/fill_in

#dim = 4
loop_opt,1,2
0,trace,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] needwun/trace
set_directive_pipeline needwun/trace
