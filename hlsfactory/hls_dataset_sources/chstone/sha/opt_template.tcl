set_directive_unroll -factor 16 sha_transform/trans_lp1

#dim = 8
array_partition,1,[1 2 4 8 16 32 64 80],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 0 "sha_transform" W 

#dim = 2
loop_opt,1,1
0,main_loop,,unroll,[2]
set_directive_unroll -factor [factor] sha_stream/main_loop

#dim = 6
loop_opt,1,6
0,sha_transform,pipeline,unroll,[2 4 8 16 20]
set_directive_unroll -factor [factor] sha_transform/trans_lp2
set_directive_unroll -factor [factor] sha_transform/trans_lp3
set_directive_unroll -factor [factor] sha_transform/trans_lp4
set_directive_pipeline sha_transform/trans_lp2
set_directive_pipeline sha_transform/trans_lp3
set_directive_pipeline sha_transform/trans_lp4


loop_opt,1,4
0,sha_transform,pipeline,unroll,[2 4 8 16 20]
set_directive_unroll -factor [factor] sha_transform/trans_lp5
set_directive_unroll -factor [factor] sha_transform/trans_lp6
set_directive_pipeline sha_transform/trans_lp5
set_directive_pipeline sha_transform/trans_lp6
