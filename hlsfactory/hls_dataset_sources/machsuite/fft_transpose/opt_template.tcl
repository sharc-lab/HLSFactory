set_directive_resource -core RAM_1P_BRAM "fft1D_512" work_x
set_directive_resource -core RAM_1P_BRAM "fft1D_512" work_y

#dim = 4
array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "fft1D_512" work_x
set_directive_array_partition -factor [factor] -type [type] -dim 1 "fft1D_512" work_y
set_directive_array_partition -factor [factor] -type [type] -dim 1 "fft1D_512" DATA_x
set_directive_array_partition -factor [factor] -type [type] -dim 1 "fft1D_512" DATA_y

#dim = 4 
loop_opt,1,12
0,loop1,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] fft1D_512/loop1
set_directive_unroll -factor [factor] fft1D_512/loop2
set_directive_unroll -factor [factor] fft1D_512/loop3
set_directive_unroll -factor [factor] fft1D_512/loop4
set_directive_unroll -factor [factor] fft1D_512/loop5
set_directive_unroll -factor [factor] fft1D_512/loop6
set_directive_pipeline fft1D_512/loop1
set_directive_pipeline fft1D_512/loop2
set_directive_pipeline fft1D_512/loop3
set_directive_pipeline fft1D_512/loop4
set_directive_pipeline fft1D_512/loop5
set_directive_pipeline fft1D_512/loop6

#dim = 4
loop_opt,1,4
0,loop7,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] fft1D_512/loop7
set_directive_pipeline fft1D_512/loop7
set_directive_unroll -factor [factor] fft1D_512/loop8
set_directive_pipeline fft1D_512/loop8

#dim = 4
loop_opt,1,6
0,loop9,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] fft1D_512/loop9
set_directive_pipeline fft1D_512/loop9
set_directive_unroll -factor [factor] fft1D_512/loop10
set_directive_pipeline fft1D_512/loop10
set_directive_unroll -factor [factor] fft1D_512/loop11
set_directive_pipeline fft1D_512/loop11
