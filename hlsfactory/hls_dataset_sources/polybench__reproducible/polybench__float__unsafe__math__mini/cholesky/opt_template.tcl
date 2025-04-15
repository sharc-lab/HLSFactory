set_directive_resource -core RAM_1P "kernel_cholesky" A

array_partition,1,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_cholesky" A

loop_opt,6,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp3,pipeline,unroll,[1 2 4 8]
3,lp3,,unroll,[1 2 4 8]
4,lp4,pipeline,unroll,[1 2 4 8]
5,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_cholesky/[name]
set_directive_pipeline kernel_cholesky/[name]
