# Example usage:
from concrete_design.csynth_analyzer import CSynthAnalyzer
from concrete_design.input_c_analyzer import InputCAnalyzer

inputc_analyzer = InputCAnalyzer("../0_work_dir/0_concrete_design/FFT.cpp")
funcs = inputc_analyzer.generate_input_c_hierarchy()
inputc_analyzer.save_hierarchy_to_json()

csynth_analyzer = CSynthAnalyzer('../0_work_dir/0_concrete_design/')
metrics = csynth_analyzer.parse_csynth_report()
csynth_analyzer.save_csynth_module_info_to_json()

csynth_analyzer.parse_transform_messages()
csynth_analyzer.save_transform_messages_to_json()

csynth_analyzer.parse_llvm_ir_hierarchy()
csynth_analyzer.save_llvm_ir_hierarchy_to_json()
csynth_analyzer.save_pretty_print_llvm_ir_hierarchy_to_txt()



# csynth_analyzer.analyze_C_to_RTL_module_mapping("../0_work_dir/0_concrete_design/logs/input_c_hierarchy.json")
