set_directive_resource -core RAM_1P_BRAM "ms_mergesort" a

#dim = 4
array_partition,1,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "ms_mergesort" a

#dim = 11 
loop_opt,2,2
0,mergesort_label2,pipeline,unroll,[2 4 8 16 32 64]
1,mergesort_label1,pipeline,unroll,[2 4 8 16]
set_directive_unroll -factor [factor] ms_mergesort/[name]
set_directive_pipeline ms_mergesort/[name]

#dim = 7
loop_opt,1,6
0,merge,pipeline,unroll,[2 4 8 16 32 64]
set_directive_unroll -factor [factor] merge/merge_label1
set_directive_unroll -factor [factor] merge/merge_label2
set_directive_unroll -factor [factor] merge/merge_label3
set_directive_pipeline merge/merge_label1
set_directive_pipeline merge/merge_label2
set_directive_pipeline merge/merge_label3
