#select memory resources
set_directive_resource -core RAM_1P "bfs" nodes
set_directive_resource -core RAM_1P "bfs" edges
set_directive_resource -core RAM_1P "bfs" level
set_directive_resource -core RAM_1P "bfs" level_counts

#io_partition and array partition, dimensionality 10
array_partition,3,[1 2 4 8 16 32 64 100 128 256],[cyclic]
set_directive_array_partition -factor [factor] -type [type] "bfs" nodes 
set_directive_array_partition -factor [factor] -type [type] "bfs" edges
set_directive_array_partition -factor [factor] -type [type] "bfs" queue


array_partition,1,[1 2 5 10],[cyclic]
set_directive_array_partition -factor [factor] -type [type] "bfs" levels


#loop pipeline and unroll, expected dimensionality is 7
#loop_opt,2,2
#0,loop_queue,pipeline,unroll,[2 4  8 16 32 64 128 256]
loop_opt,1,2
0,loop_neighbors,pipeline,unroll,[2 4 8 16 32 64 128]
set_directive_pipeline bfs/[name]
set_directive_unroll -factor [factor] bfs/[name]

