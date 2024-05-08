

#dim = 7, buffers
array_partition,2,[1 2 4 8 16 64 120],[block]
set_directive_array_partition -factor [factor] -type [type] -dim 0 "DigitRec" knn_set
set_directive_array_partition -factor [factor] -type [type] -dim 0 "DigitRec" training_set

#dim = 4
loop_opt,1,8
0,lp,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] DigitRec/lp1
set_directive_unroll -factor [factor] DigitRec/lp2
set_directive_unroll -factor [factor] DigitRec/lp3
set_directive_unroll -factor [factor] DigitRec/lp4
set_directive_pipeline DigitRec/lp1
set_directive_pipeline DigitRec/lp2
set_directive_pipeline DigitRec/lp3
set_directive_pipeline DigitRec/lp4

#dim = 9 
loop_opt,2,2
0,LANES,pipeline,unroll,[2 4 8 16 32 64 120]
1,TRAINING_LOOP,pipeline,,[2]
set_directive_unroll -factor [factor] DigitRec/[name]
set_directive_pipeline DigitRec/[name]
