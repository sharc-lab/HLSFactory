#dim = 6, Line buffer
array_partition,1,[1 2 4 8 16 24],[block]
set_directive_array_partition -factor [factor] -type [type] -dim 1 "processImage" L

#dim = 5
array_partition,3,[1 5 25 125 625],[complete]
set_directive_array_partition -factor [factor] -type [type] -dim 0 "processImage" I
set_directive_array_partition -factor [factor] -type [type] -dim 0 "processImage" II
set_directive_array_partition -factor [factor] -type [type] -dim 0 "processImage" SI

#dim = 6 
loop_opt,1,8
0,IMAGE,,unroll,[2 4 8 16 25]
set_directive_unroll -factor [factor] processImage/SetIIu
set_directive_unroll -factor [factor] processImage/SetIIj
set_directive_unroll -factor [factor] processImage/SetIj
set_directive_unroll -factor [factor] processImage/SetIi
set_directive_unroll -factor [factor] processImage/Ilast
set_directive_unroll -factor [factor] processImage/LineBuf
set_directive_unroll -factor [factor] cascadeClassifier/COPY_LOOP1
set_directive_unroll -factor [factor] cascadeClassifier/COPY_LOOP2
