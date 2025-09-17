#!/usr/bin/env python3

from concrete_design.c_to_rtl_mapper import CToRTLMapper
import json

# Create mapper for work_9 directory
mapper = CToRTLMapper("/home/cy/github/DSE-HLS-FFT/0_work_dir/dse_run/work_9/logs")

if mapper.load_json_files():
    mapper.build_c_hierarchy_index()
    mapper.build_llvm_hierarchy_index()
    
    mapper.map_simple_cases()
    mapper.map_line_based_cases()
    mapper.map_outlined_cases()
    mapper.map_function_level_cases()
    mapper.map_flattened_cases()
    mapper.apply_legalization()
    mapper.map_unmapped_to_parent()
    
    print("=== PARENT MAPPED MODULES (work_9) ===")
    for module_name, mapping in mapper.final_mappings.items():
        if mapping.get("type") == "unmapped_merged_parent":
            print(f"\n{module_name}:")
            print(json.dumps(mapping, indent=2))