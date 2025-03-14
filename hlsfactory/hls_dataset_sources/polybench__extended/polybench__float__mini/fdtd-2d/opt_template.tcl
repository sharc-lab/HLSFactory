set_directive_resource -core RAM_1P "kernel_fdtd_2d" ex
set_directive_resource -core RAM_1P "kernel_fdtd_2d" ey
set_directive_resource -core RAM_1P "kernel_fdtd_2d" hz
set_directive_resource -core RAM_1P "kernel_fdtd_2d" _fict_

array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_fdtd_2d" ex
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_fdtd_2d" ey
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_fdtd_2d" hz
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_fdtd_2d" _fict_

loop_opt,9,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
3,lp5,pipeline,unroll,[1 2 4 8]
4,lp6,pipeline,unroll,[1 2 4 8]
5,lp6,,unroll,[1 2 4 8]
6,lp7,pipeline,unroll,[1 2 4 8]
7,lp8,pipeline,unroll,[1 2 4 8]
8,lp8,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_fdtd_2d/[name]
set_directive_pipeline kernel_fdtd_2d/[name]