set_directive_resource -core RAM_1P "kernel_gemver" A
set_directive_resource -core RAM_1P "kernel_gemver" w
set_directive_resource -core RAM_1P "kernel_gemver" x

array_partition,9,[1 2 4 8],[cyclic]
set_directive_array_partition -type [type] -factor [factor] -dim 2 "kernel_gemver" A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" u1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" v1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" u2
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" v2
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" w
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" y
set_directive_array_partition -type [type] -factor [factor] -dim 1 "kernel_gemver" z

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_gemver/[name]
set_directive_pipeline kernel_gemver/[name]

loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_gemver/[name]
set_directive_pipeline kernel_gemver/[name]

loop_opt,1,2
0,lp5,pipeline,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_gemver/[name]
set_directive_pipeline kernel_gemver/[name]

loop_opt,3,2
0,lp6,pipeline,unroll,[1 2 4 8]
1,lp7,pipeline,unroll,[1 2 4 8]
2,lp7,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] kernel_gemver/[name]
set_directive_pipeline kernel_gemver/[name]
