#!/usr/bin/env python3

from concrete_design.c_to_rtl_mapper import CToRTLMapper

# Create mapper
mapper = CToRTLMapper("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_0/logs")

# Load data
if mapper.load_json_files():
    mapper.build_c_hierarchy_index()
    mapper.build_llvm_hierarchy_index()
    
    print("=== DEBUG INFO ===")
    print(f"C functions: {list(mapper.c_functions_by_name.keys())}")
    print(f"LLVM modules: {list(mapper.llvm_modules.keys())}")
    
    if "legalizing" in mapper.transform_messages:
        print("\n=== LEGALIZATION RULES ===")
        for legal in mapper.transform_messages["legalizing"]:
            original = legal["original"]
            legalized = legal["legalized"]
            print(f"{original} -> {legalized}")
            print(f"  Original in LLVM? {original in mapper.llvm_modules}")
            print(f"  Legalized in csynth? {legalized in mapper.csynth_module_info.get('metrics', {})}")
    
    # Run mapping
    mapper.map_simple_cases()
    mapper.map_line_based_cases()
    mapper.map_outlined_cases()
    mapper.map_function_level_cases()
    mapper.map_flattened_cases()
    
    print(f"\n=== BEFORE LEGALIZATION ===")
    print(f"Mapped modules: {list(mapper.final_mappings.keys())}")
    
    mapper.apply_legalization()
    
    print(f"\n=== AFTER LEGALIZATION ===") 
    print(f"Mapped modules: {list(mapper.final_mappings.keys())}")
    
    mapper.map_unmapped_to_parent()
    
    print(f"\n=== AFTER PARENT MAPPING ===") 
    print(f"Mapped modules: {list(mapper.final_mappings.keys())}")
    
    # Check results
    unmapped, missing = mapper.validate_against_csynth()