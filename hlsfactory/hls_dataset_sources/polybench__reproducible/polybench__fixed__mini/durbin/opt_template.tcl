set_directive_resource -core RAM_1P "kernel_durbin" r
set_directive_resource -core RAM_1P "kernel_durbin" y

array_partition,2,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_durbin" r
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_durbin" y

loop_opt,7,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
3,lp3,pipeline,unroll,[1 2 4 8]
4,lp3,,unroll,[1 2 4 8]
5,lp4,pipeline,unroll,[1 2 4 8]
6,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_durbin/[name]
set_directive_pipeline kernel_durbin/[name]