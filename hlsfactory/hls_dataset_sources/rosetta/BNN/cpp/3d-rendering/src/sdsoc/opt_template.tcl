set_directive_resource -core RAM_1P_BRAM "rendering" input
set_directive_resource -core RAM_1P_BRAM "rendering" output

#dim = 4, io
array_partition,2,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "rendering" input
set_directive_array_partition -factor [factor] -type [type] -dim 1 "rendering" output

#dim = 9 
loop_opt,2,2
0,output_FB_COL,pipeline,unroll,[2 4 8 16 32 64]
1,output_FB_ROW,pipeline,unroll,[2 4]
set_directive_unroll -factor [factor] output_FB/[name]
set_directive_pipeline output_FB/[name]

#dim = 9
loop_opt,1,5
0,COL,pipeline,unroll,[2 4 8 16 32 64 128 256]
set_directive_unroll -factor [factor] coloringFB/COLORING_FB_INIT_COL
set_directive_unroll -factor [factor] zculling/ZCULLING_INIT_COL
set_directive_pipeline coloringFB/COLORING_FB_INIT_COL
set_directive_pipeline zculling/ZCULLING_INIT_COL
set_directive_array_partition -factor [factor] -type cyclic -dim 2 "rendering" frame_buffer
