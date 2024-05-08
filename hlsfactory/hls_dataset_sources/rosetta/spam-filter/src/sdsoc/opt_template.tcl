#dim = 8
loop_opt,2,4
0,DOT_INNER,pipeline,unroll,[2 4 8 16 32]
1,DOT,pipeline,unroll,[2 4]
set_directive_unroll -factor [factor] dotProduct/[name]
set_directive_pipeline dotProduct/[name]
set_directive_array_partition -factor [factor] -type cyclic "dotProduct" param
set_directive_array_partition -factor [factor] -type cyclic "dotProduct" feature

#dim = 8
loop_opt,2,4
0,GRAD_INNER,pipeline,unroll,[2 4 8 16 32]
1,GRAD,pipeline,unroll,[2 4]
set_directive_unroll -factor [factor] computeGradient/[name]
set_directive_pipeline computeGradient/[name]
set_directive_array_partition -factor [factor] -type cyclic "computeGradient" param
set_directive_array_partition -factor [factor] -type cyclic "computeGradient" feature

#dim = 8
loop_opt,2,4
0,UPDATE_INNER,pipeline,unroll,[2 4 8 16 32]
1,UPDATE,pipeline,unroll,[2 4]
set_directive_unroll -factor [factor] updateParameter/[name]
set_directive_pipeline updateParameter/[name]
set_directive_array_partition -factor [factor] -type cyclic "updateParameter" param
set_directive_array_partition -factor [factor] -type cyclic "updateParameter" feature
