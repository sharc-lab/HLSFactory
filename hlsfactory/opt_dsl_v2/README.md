# Updated OptDSL

This is a potential replacement/alternative for the current version of OptDSL. It's a Pythonic representation of a few basic HLS optimizations, hopefully improving ease of use and generation with LLMs. It relies on three functions:

### Pipeline
`pipeline(label: str, function: str)`

Pipelines a loop pointed to by "label" in "function".

### Unroll
`unroll(label: str, function: str, factor: int)`

Unrolls a loop pointed to by "label" in "function" by a factor of "factor".

### Partition
`partition(array_var: str, function: str, partition_type: str, factor: int, dim: int)`

Partitions an array, "array_var",in "function", according to the "partition_type", "factor", and "dim".

These functions can then be used as in any Python program.

## Example
### OptDSLv1
```
array_partition,7,[1 2 4 8],cyclic
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" A
set_directive_array_partition -type [type] -factor [factor] -dim 2 "atax" buff_A
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" tmp1
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" buff_x
set_directive_array_partition -type [type] -factor [factor] -dim 1 "atax" buff_y_out
set_directive_unroll -factor [factor] "atax/lprd_2"
set_directive_unroll -factor [factor] "atax/lpwr_1"

set_directive_pipeline "atax/lprd_2"
set_directive_pipeline "atax/lpwr_1"

loop_opt,3,2
0,lp1,pipeline,unroll,[1 2 4 8]
1,lp2,pipeline,unroll,[1 2 4 8]
2,lp2,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] atax/[name]
set_directive_pipeline atax/[name]

loop_opt,3,2
0,lp3,pipeline,unroll,[1 2 4 8]
1,lp4,pipeline,unroll,[1 2 4 8]
2,lp4,,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] atax/[name]
set_directive_pipeline atax/[name]
```

### OptDSLv2
```
for factor in [1, 2, 4, 8]:
	for partition_type in ["cycle"]:
		partition("A", "atax", partition_type, factor, 2)
		partition("buff_A", "atax", partition_type, factor, 2)
		partition("tmp1", "atax", partition_type, factor, 1)
		partition("buff_x", "atax", partition_type, factor, 1)
		partition("buff_y_out", "atax", partition_type, factor, 1)
	unroll("lprd_2", "atax", factor)
unroll("lpwr_1", "atax", factor)

pipeline("lprd_2", "atax")
pipeline("lpwr_1", "atax")

for factor in [1, 2, 4, 8]:
	unroll("lp1", "atax", factor)
	unroll("lp2", "atax", factor)

pipeline("lp1", "atax")
pipeline("lp2", "atax")

for factor in [1, 2, 4, 8]:
	unroll("lp3", "atax", factor)
	unroll("lp4", "atax", factor)

pipeline("lp3", "atax")
pipeline("lp4", "atax")
```

This does not mimic all functionality of OptDSLv1, namely the ability to include other TCL directives in the template. Work in progress.