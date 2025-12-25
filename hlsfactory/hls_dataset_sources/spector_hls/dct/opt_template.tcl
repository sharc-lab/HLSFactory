# ---------------------------
# Global setup
# ---------------------------
set_top DCT
create_clock -period 10.0

# ---------------------------
# Array partition exploration
# ---------------------------
array_partition,2,[1 2 4],block
set_directive_array_partition -type [type] -factor [factor] -dim 1 DCT8_auto C
set_directive_array_partition -type [type] -factor [factor] -dim 1 DCT8_auto buf

# ---------------------------
# Loop-level exploration
# ---------------------------

# DCT8 k-loop
loop_opt,1,1
0,k_loop,pipeline,unroll,[1 2 4 8]
set_directive_unroll -factor [factor] DCT8_auto/[name]
set_directive_pipeline DCT8_auto/[name]

# DCT outer i-loop
loop_opt,1,1
0,i_loop,pipeline,unroll,[1 2 4]
set_directive_unroll -factor [factor] DCT/[name]
set_directive_pipeline DCT/[name]

# DCT outer j-loop
loop_opt,1,1
0,j_loop,pipeline,unroll,[1 2 4]
set_directive_unroll -factor [factor] DCT/[name]
set_directive_pipeline DCT/[name]
