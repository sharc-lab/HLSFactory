set_directive_resource -core RAM_1P_BRAM "viterbi" obs
set_directive_resource -core RAM_1P_BRAM "viterbi" init
set_directive_resource -core RAM_1P_BRAM "viterbi" transition
set_directive_resource -core RAM_1P_BRAM "viterbi" emission
set_directive_resource -core RAM_1P_BRAM "viterbi" path

#dim = 4, io
array_partition,5,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "viterbi" obs
set_directive_array_partition -factor [factor] -type [type] -dim 1 "viterbi" init
set_directive_array_partition -factor [factor] -type [type] -dim 1 "viterbi" transition
set_directive_array_partition -factor [factor] -type [type] -dim 1 "viterbi" emission
set_directive_array_partition -factor [factor] -type [type] -dim 1 "viterbi" path

# dim = 6, internal buffer
array_partition,1,[1 2 4 8 16 32],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 2 "viterbi" llike

#dim = 6 
loop_opt,1,4
0,init/timestep,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] viterbi/L_init
set_directive_unroll -factor [factor] viterbi/L_prev_state
set_directive_pipeline viterbi/L_init
set_directive_pipeline viterbi/L_prev_state

#dim = 6 
loop_opt,1,4
0,end/backtrack,pipeline,unroll,[2 4 8 16 32]
set_directive_unroll -factor [factor] viterbi/L_end
set_directive_unroll -factor [factor] viterbi/L_state
set_directive_pipeline viterbi/L_end
set_directive_pipeline viterbi/L_state
