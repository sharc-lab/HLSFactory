set_directive_inline conv3x3b
set_directive_inline encode_bit
set_directive_inline process_word
set_directive_loop_tripcount -min 1 -max 512 top/LOOP_DMEM_I
set_directive_loop_tripcount -min 1 -max 1 top/LOOP_DMEM_O
set_directive_loop_tripcount -min 1 -max 1 top/LOOP_IMG_BATCH
set_directive_loop_tripcount -min 1 -max 512 bin_conv/LOOP_PHASES
set_directive_array_partition top dmem           -dim 1 -type complete
set_directive_array_partition top dmem           -dim 2 -type complete
set_directive_array_partition top wt_mem         -dim 1 -type complete


set_directive_array_partition bin_conv line_buffer     -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv conv_params     -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv fixed_buffer    -dim 2 -type complete -factor [factor]
set_directive_array_partition bin_conv fixed_temp      -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv word_buffer     -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv old_word_buffer -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv lb              -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv rb              -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv wt_word_buffer  -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_conv conv_out_buffer -dim 0 -type complete -factor [factor]
set_directive_array_partition fp_conv win       -dim 0 -type complete -factor [factor]
set_directive_array_partition fp_conv lbuf      -dim 0 -type complete -factor [factor]
set_directive_array_partition fp_conv outwords  -dim 0 -type complete -factor [factor]
set_directive_array_partition fp_conv wtbuf     -dim 0 -type complete -factor [factor]
set_directive_array_partition bin_fc  sum_m     -dim 0 -type complete -factor [factor]

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
