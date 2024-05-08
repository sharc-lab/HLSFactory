set_directive_unroll -factor 8 Autocorrelation/autocol_lp3
set_directive_unroll -factor 8 Reflection_coefficients/Reflc_lp1
set_directive_unroll -factor 8 Reflection_coefficients/Reflc_lp2
set_directive_unroll -factor 8 Reflection_coefficients/Reflc_lp3
set_directive_array_partition -type complete -dim 0 "Reflection_coefficients" ACF 
set_directive_array_partition -type complete -dim 0 "Reflection_coefficients" P 
set_directive_array_partition -type complete -dim 0 "Reflection_coefficients" K 
set_directive_array_partition -type complete -dim 0 "Autocorrelation" s 
set_directive_array_partition -type complete -dim 0 "Autocorrelation" L_ACF 

#dim = 7
loop_opt,1,6
0,Autocorrelation,pipeline,unroll,[2 4 8 16 32 64]
set_directive_unroll -factor [factor] Autocorrelation/autocol_lp1
set_directive_unroll -factor [factor] Autocorrelation/autocol_lp2
set_directive_unroll -factor [factor] Autocorrelation/autocol_lp4
set_directive_pipeline Autocorrelation/autocol_lp1
set_directive_pipeline Autocorrelation/autocol_lp2
set_directive_pipeline Autocorrelation/autocol_lp4

#dim = 4
loop_opt,1,2
0,Transformation_to_Log_Area_Ratios,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] Transformation_to_Log_Area_Ratios/trans_lp
set_directive_pipeline Transformation_to_Log_Area_Ratios/trans_lp

#dim = 4
loop_opt,1,2
0,gsm_div,pipeline,unroll,[2 4 8]
set_directive_unroll -factor [factor] gsm_div/gsmdiv_lp
set_directive_pipeline gsm_div/gsmdiv_lp
