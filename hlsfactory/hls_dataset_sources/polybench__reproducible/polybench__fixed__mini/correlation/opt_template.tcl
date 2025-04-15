set_directive_resource -core RAM_1P "kernel_correlation" data
set_directive_resource -core RAM_1P "kernel_correlation" corr
set_directive_resource -core RAM_1P "kernel_correlation" mean
set_directive_resource -core RAM_1P "kernel_correlation" stddev

array_partition,4,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_correlation" data
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_correlation" corr
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_correlation" mean
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_correlation" stddev

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_correlation/[name]
set_directive_pipeline kernel_correlation/[name]

loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_correlation/[name]
set_directive_pipeline kernel_correlation/[name]

loop_opt,3,2
0,lp5,pipeline,unroll,[1 2 4 8]
1,lp6,pipeline,unroll,[1 2 4 8]
2,lp6,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_correlation/[name]
set_directive_pipeline kernel_correlation/[name]

loop_opt,3,2
0,lp8,pipeline,unroll,[1 2 4 8]
1,lp9,pipeline,unroll,[1 2 4 8]
2,lp9,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_correlation/[name]
set_directive_pipeline kernel_correlation/[name]