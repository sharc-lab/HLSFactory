set_directive_resource -core RAM_1P "kernel_covariance" data
set_directive_resource -core RAM_1P "kernel_covariance" cov
set_directive_resource -core RAM_1P "kernel_covariance" mean

array_partition,3,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_covariance" data
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_covariance" cov
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_covariance" mean

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_covariance/[name]
set_directive_pipeline kernel_covariance/[name]

loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_covariance/[name]
set_directive_pipeline kernel_covariance/[name]

loop_opt,3,2
0,lp6,pipeline,unroll,[1 2 4 8]
1,lp7,pipeline,unroll,[1 2 4 8]
2,lp7,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_covariance/[name]
set_directive_pipeline kernel_covariance/[name]
