set_directive_resource -core RAM_1P_BRAM "ss_sort" a
set_directive_resource -core RAM_1P_BRAM "ss_sort" b
set_directive_resource -core RAM_1P_BRAM "ss_sort" bucket
set_directive_resource -core RAM_1P_BRAM "ss_sort" sum

#dim = 4
array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ss_sort" a
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ss_sort" b
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ss_sort" bucket
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ss_sort" sum

#dim = 8 
loop_opt,2,2
0,hist_2,pipeline,unroll,[2 4]
1,hist_1,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] hist/[name]
set_directive_pipeline hist/[name]

#dim = 6
loop_opt,1,6
0,scan,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] local_scan/local_1
set_directive_unroll -factor [factor] sum_scan/sum_1
set_directive_unroll -factor [factor] last_step_scan/last_1
set_directive_pipeline local_scan/local_1
set_directive_pipeline sum_scan/sum_1
set_directive_pipeline last_step_scan/last_1

#dim = 6
loop_opt,1,2
0,update,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] update/update_1
set_directive_pipeline update/update_1
