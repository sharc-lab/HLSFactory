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

This does not mimic all functionality of OptDSLv1, namely the ability to include other TCL directives in the template. Work in progress.